#include <iostream>
#include <cstdlib>
#include <robot_dart/robot_dart_simu.hpp>
#include <robot_dart/position_control.hpp>

#ifdef GRAPHIC
#include <robot_dart/graphics.hpp>

struct Params {
    struct graphics : robot_dart::defaults::graphics {
        RS_PARAM_ARRAY(int, resolution, 1280, 720);
    };
};
#endif

int main()
{
    auto global_robot = std::make_shared<robot_dart::Robot>("res/models/nao.urdf");
    auto g_robot = global_robot->clone();
    g_robot->skeleton()->setPosition(5, 0.35);

    std::vector<double> ctrl;

    // running a simulation on a fixed robot
    ctrl.resize(50, 0);
    g_robot->fix_to_world();
#ifdef GRAPHIC
    robot_dart::RobotDARTSimu<robot_dart::robot_control<robot_dart::PositionControl>,
        robot_dart::graphics<robot_dart::Graphics<Params>>,
        robot_dart::collision<dart::collision::FCLCollisionDetector>>
        simu(ctrl, g_robot);
    auto g = simu.graphics();
    g->free_camera();
#else
    robot_dart::RobotDARTSimu<robot_dart::robot_control<robot_dart::PositionControl>,
        robot_dart::collision<dart::collision::FCLCollisionDetector>>
        simu(ctrl, g_robot);
#endif
    simu.add_floor();
    simu.run(2);

    // releasing the robot from a fixed point and simulating
    Eigen::Vector6d pose;
    pose << 0, 0, 0, 0, 0, 0.35;
    g_robot->free_from_world(pose);
    ctrl.resize(56, 0);
    simu.controller().set_parameters(ctrl);
    simu.controller().init();
    simu.run(2);

    global_robot.reset();
    g_robot.reset();
    return 0;
}