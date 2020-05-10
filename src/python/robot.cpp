#include "robot_dart.hpp"

#include <pybind11/eigen.h>
#include <pybind11/stl.h>

#include <robot_dart/robot.hpp>

#include <robot_dart/control/robot_control.hpp>

namespace robot_dart {
    namespace python {
        void py_robot(py::module& m)
        {
            using namespace robot_dart;
            // Robot class
            py::class_<Robot, std::shared_ptr<Robot>>(m, "Robot")
                .def(py::init<const std::string&, const std::vector<std::pair<std::string, std::string>>&, const std::string&, bool>())
                .def(py::init<const std::string&, const std::string&, bool>())
                .def(py::init<dart::dynamics::SkeletonPtr, const std::string&>())

                .def("clone", &Robot::clone)
                .def("skeleton", &Robot::skeleton)

                .def("name", &Robot::name)

                .def("update", &Robot::update)

                .def("reinit_controllers", &Robot::reinit_controllers)
                .def("num_controllers", &Robot::num_controllers)
                .def("controllers", &Robot::controllers)
                .def("active_controllers", &Robot::active_controllers)
                .def("controller", &Robot::controller)
                .def("add_controller", &Robot::add_controller, py::keep_alive<2, 1>())
                .def("remove_controller", (void (Robot::*)(const std::shared_ptr<control::RobotControl>&)) & Robot::remove_controller)
                .def("remove_controller", (void (Robot::*)(size_t)) & Robot::remove_controller)
                .def("clear_controllers", &Robot::clear_controllers)

                .def("fix_to_world", &Robot::fix_to_world)
                .def("free_from_world", &Robot::free_from_world)
                .def("fixed", &Robot::fixed)
                .def("free", &Robot::free)

                .def("set_actuator_type", &Robot::set_actuator_type)
                .def("set_actuator_types", (void (Robot::*)(const std::vector<dart::dynamics::Joint::ActuatorType>&)) & Robot::set_actuator_types)
                .def("set_actuator_types", (void (Robot::*)(dart::dynamics::Joint::ActuatorType)) & Robot::set_actuator_types)

                .def("actuator_type", &Robot::actuator_type)
                .def("actuator_types", &Robot::actuator_types)

                .def("set_position_enforced", (void (Robot::*)(size_t, bool)) & Robot::set_position_enforced)
                .def("set_position_enforced", (void (Robot::*)(const std::vector<bool>&)) & Robot::set_position_enforced)
                .def("set_position_enforced", (void (Robot::*)(bool)) & Robot::set_position_enforced)

                .def("position_enforced", (bool (Robot::*)(size_t) const) & Robot::position_enforced)
                .def("position_enforced", (std::vector<bool>(Robot::*)() const) & Robot::position_enforced)

                .def("set_damping_coeff", &Robot::set_damping_coeff)
                .def("set_damping_coeffs", (void (Robot::*)(const std::vector<double>&)) & Robot::set_damping_coeffs)
                .def("set_damping_coeffs", (void (Robot::*)(double)) & Robot::set_damping_coeffs)

                .def("damping_coeff", &Robot::damping_coeff)
                .def("damping_coeffs", &Robot::damping_coeffs)

                .def("set_cfriction_coeff", &Robot::set_cfriction_coeff)
                .def("set_cfriction_coeffs", (void (Robot::*)(const std::vector<double>&)) & Robot::set_cfriction_coeffs)
                .def("set_cfriction_coeffs", (void (Robot::*)(double)) & Robot::set_cfriction_coeffs)

                .def("cfriction_coeff", &Robot::cfriction_coeff)
                .def("cfriction_coeffs", &Robot::cfriction_coeffs)

                .def("set_base_pose", &Robot::set_base_pose)

                .def("body_pos", &Robot::body_pos)
                .def("body_rot", &Robot::body_rot)
                .def("body_trans", &Robot::body_trans)

                .def("set_color_mode", (void (Robot::*)(dart::dynamics::MeshShape::ColorMode)) & Robot::set_color_mode)
                .def("set_color_mode", (void (Robot::*)(dart::dynamics::MeshShape::ColorMode, const std::string&)) & Robot::set_color_mode)

                .def_static("create_box", &Robot::create_box)
                .def_static("create_ellipsoid", &Robot::create_ellipsoid);
        }
    } // namespace python
} // namespace robot_dart