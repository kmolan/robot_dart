#include "robot_dart.hpp"

#include <pybind11/eigen.h>
#include <pybind11/functional.h>
#include <pybind11/stl.h>

#include <robot_dart/robot_dart_simu.hpp>

#ifdef GRAPHIC
#include <robot_dart/gui/magnum/camera_osr.hpp>
#include <robot_dart/gui/magnum/graphics.hpp>
#include <robot_dart/gui/magnum/windowless_gl_application.hpp>
#endif

namespace robot_dart {
    namespace python {
#ifdef GRAPHIC
        void py_gui(py::module& m)
        {
            auto sm = m.def_submodule("gui");

            using namespace robot_dart;

            // Helper definitions and classes
            using BaseGraphics = gui::magnum::Graphics<gui::magnum::GlfwApplication>;
            using BaseWindowlessGraphics = gui::magnum::Graphics<gui::magnum::WindowlessGLApplication>;

            class Graphics : public BaseGraphics {
            public:
                using BaseGraphics::BaseGraphics;
            };

            class WindowlessGraphics : public BaseWindowlessGraphics {
            public:
                using BaseWindowlessGraphics::BaseWindowlessGraphics;
            };

            py::class_<gui::Base, std::shared_ptr<gui::Base>>(sm, "Base");
            py::class_<BaseGraphics, gui::Base, std::shared_ptr<BaseGraphics>>(sm, "BaseGraphics");
            py::class_<BaseWindowlessGraphics, gui::Base, std::shared_ptr<BaseWindowlessGraphics>>(sm, "BaseWindowlessGraphics");
            py::class_<gui::magnum::BaseApplication>(sm, "BaseApplication");

            // Graphics class
            py::class_<Graphics, BaseGraphics, std::shared_ptr<Graphics>>(sm, "Graphics")
                .def(py::init<const dart::simulation::WorldPtr&, unsigned int, unsigned int, bool, bool, const std::string&>())

                .def("done", &Graphics::done)
                .def("refresh", &Graphics::refresh)
                .def("set_render_period", &Graphics::set_render_period)
                .def("set_enable", &Graphics::set_enable)

                .def("look_at", &Graphics::look_at)

                .def("clear_lights", &Graphics::clear_lights)
                .def("add_light", &Graphics::add_light, py::keep_alive<2, 1>())
                .def("lights", &Graphics::lights)
                .def("num_lights", &Graphics::num_lights)
                .def("light", &Graphics::light)

                .def("set_recording", &Graphics::set_recording)
                .def("recording", &Graphics::recording)
                .def("recording_depth", &Graphics::recording_depth)

                .def("is_shadowed", &Graphics::is_shadowed)
                .def("enable_shadows", &Graphics::enable_shadows)

                // Magnum::Image2D* magnum_image()
                .def("image", &Graphics::image)
                .def("depth_image", &Graphics::depth_image)
                .def("raw_depth_image", &Graphics::raw_depth_image)

                .def("set_speed", &Graphics::set_speed)
                .def("set_near_plane", &Graphics::set_near_plane)
                .def("set_far_plane", &Graphics::set_far_plane)
                .def("set_fov", &Graphics::set_fov)
                .def("set_camera_params", &Graphics::set_camera_params)

                .def("speed", &Graphics::speed)
                .def("near_plane", &Graphics::near_plane)
                .def("far_plane", &Graphics::far_plane)
                .def("fov", &Graphics::fov)

                .def("magnum_app", &Graphics::magnum_app, py::return_value_policy::reference);

            // WindowlessGraphics class
            py::class_<WindowlessGraphics, BaseWindowlessGraphics, std::shared_ptr<WindowlessGraphics>>(sm, "WindowlessGraphics")
                .def(py::init<const dart::simulation::WorldPtr&, unsigned int, unsigned int, bool, bool, const std::string&>())

                .def("done", &WindowlessGraphics::done)
                .def("refresh", &WindowlessGraphics::refresh)
                .def("set_render_period", &WindowlessGraphics::set_render_period)
                .def("set_enable", &WindowlessGraphics::set_enable)

                .def("look_at", &WindowlessGraphics::look_at)

                .def("clear_lights", &WindowlessGraphics::clear_lights)
                .def("add_light", &WindowlessGraphics::add_light, py::keep_alive<2, 1>())
                .def("lights", &WindowlessGraphics::lights)
                .def("num_lights", &WindowlessGraphics::num_lights)
                .def("light", &WindowlessGraphics::light)

                .def("set_recording", &WindowlessGraphics::set_recording)
                .def("recording", &WindowlessGraphics::recording)
                .def("recording_depth", &WindowlessGraphics::recording_depth)

                .def("is_shadowed", &WindowlessGraphics::is_shadowed)
                .def("enable_shadows", &WindowlessGraphics::enable_shadows)

                // Magnum::Image2D* magnum_image()
                .def("image", &WindowlessGraphics::image)
                .def("depth_image", &WindowlessGraphics::depth_image)
                .def("raw_depth_image", &WindowlessGraphics::raw_depth_image)

                .def("set_speed", &WindowlessGraphics::set_speed)
                .def("set_near_plane", &WindowlessGraphics::set_near_plane)
                .def("set_far_plane", &WindowlessGraphics::set_far_plane)
                .def("set_fov", &WindowlessGraphics::set_fov)
                .def("set_camera_params", &WindowlessGraphics::set_camera_params)

                .def("speed", &WindowlessGraphics::speed)
                .def("near_plane", &WindowlessGraphics::near_plane)
                .def("far_plane", &WindowlessGraphics::far_plane)
                .def("fov", &WindowlessGraphics::fov)

                .def("magnum_app", &WindowlessGraphics::magnum_app, py::return_value_policy::reference);

            sm.def(
                "run_with_gl_context", +[](const std::function<void()>& func, size_t wait_ms) {
                    get_gl_context_with_sleep(my_context, wait_ms);
                    /* Acquire GIL before calling Python code */
                    py::gil_scoped_acquire acquire;
                    func();
                    py::gil_scoped_release release;
                    release_gl_context(my_context);
                });

            sm.def(
                "set_max_contexts", +[](size_t num_contexts) {
                    gui::magnum::GlobalData::instance()->set_max_contexts(num_contexts);
                });

            // CameraOSR class
            py::class_<gui::magnum::CameraOSR, gui::Base, std::shared_ptr<gui::magnum::CameraOSR>>(sm, "CameraOSR")
                .def(py::init<const dart::simulation::WorldPtr&, gui::magnum::BaseApplication*, size_t, size_t>())

                .def("done", &gui::magnum::CameraOSR::done)
                .def("refresh", &gui::magnum::CameraOSR::refresh)
                .def("set_render_period", &gui::magnum::CameraOSR::set_render_period)
                .def("set_enable", &gui::magnum::CameraOSR::set_enable)

                .def("look_at", &gui::magnum::CameraOSR::look_at)

                .def("render", &gui::magnum::CameraOSR::render)

                .def("set_recording", &gui::magnum::CameraOSR::set_recording)
                .def("recording", &gui::magnum::CameraOSR::recording)
                .def("recording_depth", &gui::magnum::CameraOSR::recording_depth)

                // Magnum::Image2D* magnum_image()
                // Magnum::Image2D* magnum_depth_image()

                .def("image", &gui::magnum::CameraOSR::image)
                .def("depth_image", &gui::magnum::CameraOSR::depth_image)
                .def("raw_depth_image", &gui::magnum::CameraOSR::raw_depth_image)

                .def("attach_to", &gui::magnum::CameraOSR::attach_to)

                .def("set_speed", &gui::magnum::CameraOSR::set_speed)
                .def("set_near_plane", &gui::magnum::CameraOSR::set_near_plane)
                .def("set_far_plane", &gui::magnum::CameraOSR::set_far_plane)
                .def("set_fov", &gui::magnum::CameraOSR::set_fov)
                .def("set_camera_params", &gui::magnum::CameraOSR::set_camera_params)

                .def("speed", &gui::magnum::CameraOSR::speed)
                .def("near_plane", &gui::magnum::CameraOSR::near_plane)
                .def("far_plane", &gui::magnum::CameraOSR::far_plane)
                .def("fov", &gui::magnum::CameraOSR::fov);

            // Helper functions
            sm.def("save_png_image", (void (*)(const std::string&, const gui::Image&)) & gui::save_png_image);
            sm.def("save_png_image", (void (*)(const std::string&, const gui::GrayscaleImage&)) & gui::save_png_image);

            // Material class
            using Material = gui::magnum::gs::Material;
            py::class_<Material>(sm, "Material")
                .def(py::init<const Magnum::Color4&, const Magnum::Color4&, const Magnum::Color4&, Magnum::Float>());
            // TO-DO: Add more functions
            using Light = gui::magnum::gs::Light;
            py::class_<Light>(sm, "Light")
                .def(py::init<const Magnum::Vector4&, const Material&, const Magnum::Vector3&, Magnum::Float, Magnum::Float, const Magnum::Vector4&>());
            // TO-DO: Add more functions

            sm.def("createPointLight", &gui::magnum::gs::createPointLight);
            sm.def("createSpotLight", &gui::magnum::gs::createSpotLight);
            sm.def("createDirectionalLight", &gui::magnum::gs::createDirectionalLight);
        }
#endif
    } // namespace python
} // namespace robot_dart