from launch import LaunchDescription
from launch_ros.actions import Node
from launch.substitutions import Command
from launch_ros.parameter_descriptions import ParameterValue
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    pkg_path = get_package_share_directory('battery_hardware')
    
    robot_description_content = Command([
        'xacro ', os.path.join(pkg_path, 'urdf', 'battery_robot.urdf.xacro')
    ])
    robot_description = ParameterValue(robot_description_content, value_type=str)

    controllers_yaml = os.path.join(pkg_path, 'config', 'controllers.yaml')

    control_node = Node(
        package='controller_manager',
        executable='ros2_control_node',
        parameters=[
            {'robot_description': robot_description},
            controllers_yaml
        ],
        output='screen',
    )

    return LaunchDescription([control_node])