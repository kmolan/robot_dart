#include <iostream>
#include <thread>

#include <robot_dart/robot_dart_simu.hpp>

#include <robot_dart/control/hexa_control.hpp>
#include <robot_dart/control/pd_control.hpp>

#include <robot_dart/gui/helper.hpp>
#include <robot_dart/gui/magnum/glx_application.hpp>
#include <robot_dart/gui/magnum/graphics.hpp>
#include <robot_dart/gui/magnum/gs/helper.hpp>

int main()
{
    CORRADE_RESOURCE_INITIALIZE(RobotDARTShaders);
    std::srand(std::time(NULL));

    // Load robot from URDF
    std::vector<std::pair<std::string, std::string>> packages = {{"iiwa14", std::string(RESPATH) + "/models/meshes"}};
    auto global_robot = std::make_shared<robot_dart::Robot>("res/models/iiwa14.urdf", packages);

    global_robot->fix_to_world();
    global_robot->set_position_enforced(true);

    std::vector<std::thread> workers;

    // Set maximum number of parallel GL contexts (this is GPU-dependent)
    robot_dart::gui::magnum::GlobalData::instance()->set_max_contexts(4);

    // We want 15 parallel simulations with different GL context each
    size_t N_workers = 15;
    std::mutex mutex;
    size_t id = 0;
    // Launch the workers
    for (size_t i = 0; i < N_workers; i++) {
        workers.push_back(std::thread([&] {
            mutex.lock();
            size_t index = id++;
            mutex.unlock();

            // Get the GL context -- this is a blocking call
            // will wait until one GL context is available
            get_glx_context(glx_context);

            // Do the simulation
            auto g_robot = global_robot->clone();

            robot_dart::RobotDARTSimu simu(0.001);

            std::vector<double> ctrl;
            ctrl = {0., M_PI / 3., 0., -M_PI / 4., 0., 0., 0.};

            g_robot->add_controller(std::make_shared<robot_dart::control::PDControl>(ctrl));
            std::static_pointer_cast<robot_dart::control::PDControl>(g_robot->controllers()[0])->set_pd(300., 50.);

            // Magnum graphics
            auto graphics = std::make_shared<robot_dart::gui::magnum::Graphics<robot_dart::gui::magnum::GLXApplication>>(simu.world(), 1024, 768);
            simu.set_graphics(graphics);
            // Position the camera differently for each thread to visualize the difference
            graphics->look_at({0.4 * index, 3.5, 2.}, {0., 0., 0.25});
            // record images from main camera/graphics
            graphics->set_recording(true);

            simu.add_robot(g_robot);
            simu.run(6);

            // Save the image for verification
            robot_dart::gui::save_png_image("camera_" + std::to_string(index) + ".png", robot_dart::gui::magnum::gs::rgb_from_image(graphics->image()));

            // Release the GL context for another thread to use
            release_glx_context(glx_context);
        }));
    }

    // Wait for all the workers
    for (size_t i = 0; i < workers.size(); i++) {
        workers[i].join();
    }

    global_robot.reset();
    return 0;
}