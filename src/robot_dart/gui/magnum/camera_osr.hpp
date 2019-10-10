#ifndef ROBOT_DART_GUI_MAGNUM_CAMERA_OSR_HPP
#define ROBOT_DART_GUI_MAGNUM_CAMERA_OSR_HPP

#include <robot_dart/gui/base.hpp>
#include <robot_dart/gui/magnum/base_application.hpp>
#include <robot_dart/gui/magnum/gs/helper.hpp>

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/PixelFormat.h>

namespace robot_dart {
    namespace gui {
        namespace magnum {
            class CameraOSR : public Base {
            public:
                CameraOSR(const dart::simulation::WorldPtr& world, BaseApplication* app, size_t width, size_t height);
                ~CameraOSR() {}

                bool done() const override { return _done; }
                virtual void refresh() override;
                virtual void set_render_period(double dt) override;
                void set_enable(bool enable) override { _enabled = enable; }

                void look_at(const Eigen::Vector3d& camera_pos, const Eigen::Vector3d& look_at = Eigen::Vector3d(0, 0, 0), const Eigen::Vector3d& up = Eigen::Vector3d(0, 0, 1));

                void render();

                void set_recording(bool recording) { _camera->record(recording); }
                bool recording() { return _camera->isRecording(); }

                Magnum::Image2D* magnum_image()
                {
                    if (_camera->image())
                        return &(*_camera->image());
                    return nullptr;
                }

                Image image() override
                {
                    auto image = magnum_image();
                    if (image)
                        return gs::rgb_from_image(image);
                    return Image();
                }

                virtual void attach_to(const std::string& name, const Eigen::Isometry3d& tf);

                void set_speed(const Magnum::Vector2& speed) { _camera->setSpeed(speed); }
                void set_near_plane(double near_plane) { _camera->setNearPlane(near_plane); }
                void set_far_plane(double far_plane) { _camera->setNearPlane(far_plane); }
                void set_fov(double fov) { _camera->setFOV(fov); }
                void set_camera_params(double near_plane, double far_plane, double fov, size_t width, size_t height) { _camera->setCameraParameters(near_plane, far_plane, fov, width, height); }

                Magnum::Vector2 speed() const { return _camera->speed(); }
                double nearPlane() const { return _camera->nearPlane(); }
                double farPlane() const { return _camera->farPlane(); }
                double fov() const { return _camera->fov(); }

            protected:
                Magnum::GL::Framebuffer _framebuffer{Magnum::NoCreate};
                Magnum::PixelFormat _format;

                BaseApplication* _magnum_app;
                size_t _render_period, _width, _height, _frame_counter;
                bool _enabled, _done;
                bool _attaching = false, _attached = false;
                std::string _attach_to;
                Eigen::Isometry3d _attached_tf;

                std::unique_ptr<gs::Camera> _camera;
            };
        } // namespace magnum
    } // namespace gui
} // namespace robot_dart

#endif