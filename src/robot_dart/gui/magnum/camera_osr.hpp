#ifndef ROBOT_DART_GUI_MAGNUM_CAMERA_OSR_HPP
#define ROBOT_DART_GUI_MAGNUM_CAMERA_OSR_HPP

#include <robot_dart/gui/base.hpp>
#include <robot_dart/gui/magnum/base_application.hpp>
#include <robot_dart/gui/magnum/gs/helper.hpp>

#include <Magnum/GL/Framebuffer.h>
#include <Magnum/GL/Renderbuffer.h>
#include <Magnum/PixelFormat.h>

namespace robot_dart {
    namespace gui {
        namespace magnum {
            class CameraOSR : public Base {
            public:
                static constexpr int FPS = 30;
                CameraOSR(RobotDARTSimu* simu, BaseApplication* app, size_t width, size_t height, bool draw_debug = false);
                ~CameraOSR() {}

                bool done() const override { return _done; }
                virtual void refresh() override;
                virtual void set_render_period(double dt) override;
                void set_enable(bool enable) override { _enabled = enable; }

                void look_at(const Eigen::Vector3d& camera_pos, const Eigen::Vector3d& look_at = Eigen::Vector3d(0, 0, 0), const Eigen::Vector3d& up = Eigen::Vector3d(0, 0, 1));

                void render();

                // this will use the default FPS of the camera if fps == -1
                void record_video(const std::string& video_fname, int fps = -1)
                {
                    int fps_computed = (fps == -1) ? FPS : fps;
                    _camera->record_video(video_fname, fps_computed);
                }

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

                Magnum::Image2D* magnum_depth_image()
                {
                    if (_camera->depth_image())
                        return &(*_camera->depth_image());
                    return nullptr;
                }

                // This is for visualization purposes
                GrayscaleImage depth_image() override;

                // Image filled with depth buffer values
                GrayscaleImage raw_depth_image() override;

                // // Point-cloud made from each pixel in depth image
                // // all transformations are according to the camera frame
                // std::vector<Eigen::Vector3d> point_cloud();

                virtual void attach_to(const std::string& name, const Eigen::Isometry3d& tf);

                gs::Camera& camera() { return *_camera; }
                const gs::Camera& camera() const { return *_camera; }

                bool drawing_debug() const { return _draw_debug; }
                void draw_debug(bool draw = true) { _draw_debug = draw; }

            protected:
                RobotDARTSimu* _simu;
                Magnum::GL::Framebuffer _framebuffer{Magnum::NoCreate};
                Magnum::PixelFormat _format;
                Magnum::GL::Renderbuffer _color, _depth;

                BaseApplication* _magnum_app;
                size_t _render_period, _width, _height, _frame_counter;
                bool _enabled, _done;
                bool _attaching = false, _attached = false;
                std::string _attach_to;
                Eigen::Isometry3d _attached_tf;

                std::unique_ptr<gs::Camera> _camera;

                bool _draw_debug;
            };
        } // namespace magnum
    } // namespace gui
} // namespace robot_dart

#endif
