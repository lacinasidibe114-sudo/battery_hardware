#include "battery_hardware/battery_interface.hpp"
#include <cstdio>

#include "hardware_interface/types/hardware_interface_type_values.hpp"
#include "pluginlib/class_list_macros.hpp"

namespace battery_hardware
{

hardware_interface::CallbackReturn BatteryInterface::on_init(
  const hardware_interface::HardwareInfo & info)
{
  if (hardware_interface::SensorInterface::on_init(info) !=
      hardware_interface::CallbackReturn::SUCCESS)
  {
    return hardware_interface::CallbackReturn::ERROR;
  }

  device_ = info_.hardware_parameters["device"];
  
  hw_voltage_ = 0.0;
  hw_current_ = 0.0;
  hw_power_ = 0.0;

  // Création d'un petit nœud ROS 2 pour publier le topic
  node_ = rclcpp::Node::make_shared("battery_hardware_node");
  battery_pub_ = node_->create_publisher<sensor_msgs::msg::BatteryState>("/battery_state", 10);

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn BatteryInterface::on_activate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  try {
    serial_port_.Open(device_);
    serial_port_.SetBaudRate(LibSerial::BaudRate::BAUD_115200);
    serial_port_.SetCharacterSize(LibSerial::CharacterSize::CHAR_SIZE_8);
    serial_port_.SetParity(LibSerial::Parity::PARITY_NONE);
    serial_port_.SetStopBits(LibSerial::StopBits::STOP_BITS_1);
    serial_port_.SetFlowControl(LibSerial::FlowControl::FLOW_CONTROL_NONE);
  } catch (...) {
    RCLCPP_FATAL(rclcpp::get_logger("BatteryHardware"), "Impossible d'ouvrir %s", device_.c_str());
    return hardware_interface::CallbackReturn::ERROR;
  }

  return hardware_interface::CallbackReturn::SUCCESS;
}

hardware_interface::CallbackReturn BatteryInterface::on_deactivate(
  const rclcpp_lifecycle::State & /*previous_state*/)
{
  if (serial_port_.IsOpen()) {
    serial_port_.Close();
  }
  return hardware_interface::CallbackReturn::SUCCESS;
}

std::vector<hardware_interface::StateInterface> BatteryInterface::export_state_interfaces()
{
  std::vector<hardware_interface::StateInterface> state_interfaces;
  
  std::string sensor_name = info_.sensors[0].name;

  state_interfaces.emplace_back(sensor_name, "voltage", &hw_voltage_);
  state_interfaces.emplace_back(sensor_name, "current", &hw_current_);
  state_interfaces.emplace_back(sensor_name, "power", &hw_power_);

  return state_interfaces;
}

hardware_interface::return_type BatteryInterface::read(
  const rclcpp::Time & /*time*/, const rclcpp::Duration & /*period*/)
{
  if (!serial_port_.IsOpen()) return hardware_interface::return_type::ERROR;

  try {
    std::string data;
    serial_port_.ReadLine(data, '\n', 10);
    
    float v, i, p;
    if (std::sscanf(data.c_str(), "V:%f I:%f P:%f", &v, &i, &p) == 3) {
      hw_voltage_ = v;
      hw_current_ = i;
      hw_power_ = p;

      // Publication du message ROS 2 standard
      auto msg = sensor_msgs::msg::BatteryState();
      msg.header.stamp = node_->now();
      msg.voltage = hw_voltage_;
      msg.current = hw_current_;
      msg.charge = hw_power_; // Approximation
      msg.present = true;
      battery_pub_->publish(msg);
    }
  } catch (...) {}

  return hardware_interface::return_type::OK;
}

}  // namespace battery_hardware

PLUGINLIB_EXPORT_CLASS(battery_hardware::BatteryInterface, hardware_interface::SensorInterface)