#ifndef BATTERY_HARDWARE__BATTERY_INTERFACE_HPP_
#define BATTERY_HARDWARE__BATTERY_INTERFACE_HPP_

#include <string>
#include <vector>
#include <libserial/SerialPort.h>
#include <memory>

#include "hardware_interface/sensor_interface.hpp"
#include "hardware_interface/handle.hpp"
#include "hardware_interface/hardware_info.hpp"
#include "hardware_interface/types/hardware_interface_return_values.hpp"
#include "rclcpp/rclcpp.hpp"
#include "rclcpp_lifecycle/state.hpp"
#include "sensor_msgs/msg/battery_state.hpp" // <-- Ajouté pour publier le topic

namespace battery_hardware
{

class BatteryInterface : public hardware_interface::SensorInterface
{
public:
  hardware_interface::CallbackReturn on_init(
    const hardware_interface::HardwareInfo & info) override;

  hardware_interface::CallbackReturn on_activate(
    const rclcpp_lifecycle::State & previous_state) override;

  hardware_interface::CallbackReturn on_deactivate(
    const rclcpp_lifecycle::State & previous_state) override;

  std::vector<hardware_interface::StateInterface> export_state_interfaces() override;

  hardware_interface::return_type read(
    const rclcpp::Time & time, const rclcpp::Duration & period) override;

private:
  LibSerial::SerialPort serial_port_;
  std::string device_;

  // Variables d'état
  double hw_voltage_;
  double hw_current_;
  double hw_power_;

  // Pour publier le message standard ROS 2
  rclcpp::Publisher<sensor_msgs::msg::BatteryState>::SharedPtr battery_pub_;
  rclcpp::Node::SharedPtr node_;
};

}  // namespace battery_hardware

#endif