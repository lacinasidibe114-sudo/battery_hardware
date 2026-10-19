cmake_minimum_required(VERSION 3.8)
project(battery_hardware)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# --- 1. Dépendances ROS 2 ---
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(rclcpp_lifecycle REQUIRED)
find_package(hardware_interface REQUIRED)
find_package(pluginlib REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(rclpy REQUIRED) # Ajouté pour le script Python

# --- 2. Dépendance LibSerial ---
find_package(PkgConfig REQUIRED)
pkg_check_modules(SERIAL REQUIRED libserial)

# --- 3. Compilation de la librairie C++ ---
add_library(battery_hardware SHARED
  src/battery_interface.cpp)

target_include_directories(battery_hardware PUBLIC
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
  $<INSTALL_INTERFACE:include>
  ${SERIAL_INCLUDE_DIRS})

target_link_libraries(battery_hardware PUBLIC
  hardware_interface::hardware_interface
  pluginlib::pluginlib
  rclcpp::rclcpp
  sensor_msgs::sensor_msgs
  ${SERIAL_LIBRARIES})

# --- 4. Export du plugin matériel ---
pluginlib_export_plugin_description_file(
  hardware_interface battery_hardware.xml)

# --- 5. Installation des fichiers (URDF, Config, Launch) ---
install(FILES battery_hardware.xml DESTINATION share/${PROJECT_NAME})
install(DIRECTORY include/ DESTINATION include)
install(DIRECTORY urdf DESTINATION share/${PROJECT_NAME})
install(DIRECTORY config DESTINATION share/${PROJECT_NAME})
install(DIRECTORY launch DESTINATION share/${PROJECT_NAME})

# =========================================================
# INSTALLATION DU SCRIPT PYTHON (La partie que tu demandais)
# =========================================================
install(PROGRAMS
  scripts/battery_monitor.py
  DESTINATION lib/${PROJECT_NAME}
)
# =========================================================

# --- 6. Installation de la librairie C++ ---
install(TARGETS battery_hardware
  EXPORT export_battery_hardware
  ARCHIVE DESTINATION lib
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin)

# --- 7. Tests (linters) ---
if(BUILD_TESTING)
  find_package(ament_lint_auto REQUIRED)
  set(ament_cmake_copyright_FOUND TRUE)
  set(ament_cmake_cpplint_FOUND TRUE)
  ament_lint_auto_find_test_dependencies()
endif()

ament_package()