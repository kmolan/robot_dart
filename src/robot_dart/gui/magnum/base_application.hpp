#ifndef ROBOT_DART_GUI_MAGNUM_BASE_APPLICATION_HPP
#define ROBOT_DART_GUI_MAGNUM_BASE_APPLICATION_HPP

#include <mutex>
#include <unistd.h>
#include <unordered_map>

#include <robot_dart/gui/helper.hpp>
#include <robot_dart/gui/magnum/drawables.hpp>
#include <robot_dart/gui/magnum/gs/camera.hpp>
#include <robot_dart/gui/magnum/gs/cube_map.hpp>
#include <robot_dart/gui/magnum/gs/cube_map_color.hpp>
#include <robot_dart/gui/magnum/gs/phong_multi_light.hpp>
#include <robot_dart/gui/magnum/gs/shadow_map.hpp>
#include <robot_dart/gui/magnum/gs/shadow_map_color.hpp>
#include <robot_dart/gui/magnum/types.hpp>

#include <dart/simulation/World.hpp>

#include <Magnum/GL/CubeMapTextureArray.h>
#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/TextureArray.h>
#include <Magnum/Platform/GLContext.h>
#ifndef MAGNUM_MAC_OSX
#include <Magnum/Platform/WindowlessGlxApplication.h>
#else
#include <Magnum/Platform/WindowlessCglApplication.h>
#endif

#include <Magnum/DartIntegration/World.h>

#define get_gl_context_with_sleep(name, ms_sleep)                             \
    /* Create/Get GLContext */                                                \
    Corrade::Utility::Debug name##_magnum_silence_output{nullptr};            \
    Magnum::Platform::WindowlessGLContext* name = nullptr;                    \
    while (name == nullptr) {                                                 \
        name = robot_dart::gui::magnum::GlobalData::instance()->gl_context(); \
        /* Sleep for some ms */                                               \
        usleep(ms_sleep * 1000);                                              \
    }                                                                         \
    while (!name->makeCurrent()) {                                            \
        /* Sleep for some ms */                                               \
        usleep(ms_sleep * 1000);                                              \
    }                                                                         \
                                                                              \
    Magnum::Platform::GLContext name##_magnum_context;

#define get_gl_context(name) get_gl_context_with_sleep(name, 0)

#define release_gl_context(name) robot_dart::gui::magnum::GlobalData::instance()->free_gl_context(name);

namespace robot_dart {
    namespace gui {
        namespace magnum {
            struct GlobalData {
            public:
                static GlobalData* instance()
                {
                    static GlobalData gdata;
                    return &gdata;
                }

                GlobalData(const GlobalData&) = delete;
                void operator=(const GlobalData&) = delete;

                Magnum::Platform::WindowlessGLContext* gl_context();
                void free_gl_context(Magnum::Platform::WindowlessGLContext* context);

                /* You should call this before starting to draw or after finished */
                void set_max_contexts(size_t N);

            private:
                GlobalData() = default;
                ~GlobalData() = default;

                void _create_contexts();

                std::vector<Magnum::Platform::WindowlessGLContext> _gl_contexts;
                std::vector<bool> _used;
                std::mutex _context_mutex;
                size_t _max_contexts = 4;
            };

            class BaseApplication {
            public:
                BaseApplication(bool isShadowed = true, bool drawTransparentShadows = true);
                virtual ~BaseApplication() {}

                void init(const dart::simulation::WorldPtr& world, size_t width, size_t height);

                void clearLights();
                void addLight(const gs::Light& light);
                gs::Light& light(size_t i);
                std::vector<gs::Light>& lights();
                size_t numLights() const;

                Magnum::SceneGraph::DrawableGroup3D& drawables() { return _drawables; }
                Scene3D& scene() { return _scene; }
                gs::Camera& camera() { return *_camera; }

                bool done() const;

                void lookAt(const Eigen::Vector3d& camera_pos,
                    const Eigen::Vector3d& look_at,
                    const Eigen::Vector3d& up);

                virtual void render() {}

                void updateLights(const gs::Camera& camera);
                void updateGraphics();
                void renderShadows();
                bool attachCamera(gs::Camera& camera, const std::string& name);

                void record(bool recording, bool depthRecording = false) { _camera->record(recording, depthRecording); }
                bool isRecording() { return _camera->isRecording(); }
                bool isDepthRecording() { return _camera->isDepthRecording(); }

                bool isShadowed() const { return _isShadowed; }
                bool transparentShadows() const { return _drawTransparentShadows; }
                void enableShadows(bool enable = true, bool drawTransparentShadows = false);

                Corrade::Containers::Optional<Magnum::Image2D>& image() { return _camera->image(); }

                // This is for visualization purposes
                GrayscaleImage depthImage();

                // Image filled with depth buffer values
                GrayscaleImage rawDepthImage();

            protected:
                /* Magnum */
                Scene3D _scene;
                Magnum::SceneGraph::DrawableGroup3D _drawables, _shadowed_drawables, _shadowed_color_drawables, _cubemap_drawables, _cubemap_color_drawables;
                std::unique_ptr<gs::PhongMultiLight> _color_shader, _texture_shader;

                std::unique_ptr<gs::Camera> _camera;

                bool _done = false;

                /* DART */
                std::unique_ptr<Magnum::DartIntegration::World> _dartWorld;
                std::unordered_map<Magnum::DartIntegration::Object*, ObjectStruct*> _drawableObjects;
                std::vector<Object3D*> _dartObjs;
                std::vector<gs::Light> _lights;

                /* Shadows */
                bool _isShadowed = true, _drawTransparentShadows = false;
                int _transparentSize = 0;
                std::unique_ptr<gs::ShadowMap> _shadow_shader, _shadow_texture_shader;
                std::unique_ptr<gs::ShadowMapColor> _shadow_color_shader, _shadow_texture_color_shader;
                std::unique_ptr<gs::CubeMap> _cubemap_shader, _cubemap_texture_shader;
                std::unique_ptr<gs::CubeMapColor> _cubemap_color_shader, _cubemap_texture_color_shader;
                std::vector<ShadowData> _shadowData;
                std::unique_ptr<Magnum::GL::Texture2DArray> _shadowTexture, _shadowColorTexture;
                std::unique_ptr<Magnum::GL::CubeMapTextureArray> _shadowCubeMap, _shadowColorCubeMap;
                int _shadowMapSize = 512;
                int _maxLights = 5;
                std::unique_ptr<Camera3D> _shadowCamera;
                Object3D* _shadowCameraObject;
                Corrade::PluginManager::Manager<Magnum::Trade::AbstractImporter> _importer_manager;

                void GLCleanUp();
                void prepareShadows();
            };

            template <typename T>
            inline BaseApplication* make_application(const dart::simulation::WorldPtr& world, size_t width, size_t height, const std::string& title = "DART", bool isShadowed = true, bool drawTransparentShadows = true)
            {
                int argc = 0;
                char** argv = NULL;

                return new T(argc, argv, world, width, height, title, isShadowed, drawTransparentShadows);
            }
        } // namespace magnum
    } // namespace gui
} // namespace robot_dart

#endif