import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LoadComposableNodes
from launch_ros.descriptions import ComposableNode

def launch_camera(context):
    name = LaunchConfiguration('name').perform(context)
    depthai_prefix = get_package_share_directory("depthai_ros_driver")

    params_file = LaunchConfiguration("params_file")
    parameters = [
        {
            "frame_id": name,
            "subscribe_stereo": True,  # Subscribing to stereo topics
            "subscribe_odom_info": True,
            "approx_sync": True,
            "Rtabmap/DetectionRate": "3.5",
        }
    ]

    remappings = [
        ("rgb/image", name+"/rgb/image_rect/compressed"),  # Publishing compressed RGB image
        ("rgb/camera_info", name+"/rgb/camera_info"),
        ("depth/image", name+"/stereo/image_raw/compressedDepth"),  # Publishing compressed depth image
    ]

    return [
        IncludeLaunchDescription(
            PythonLaunchDescriptionSource(
                os.path.join(depthai_prefix, 'launch', 'camera.launch.py')),
            launch_arguments={"name": name,
                              "params_file": params_file}.items()),


    ]

def generate_launch_description():
    depthai_prefix = get_package_share_directory("depthai_ros_driver")
    declared_arguments = [
        DeclareLaunchArgument("name", default_value="oak"),
        DeclareLaunchArgument("params_file", default_value=os.path.join(depthai_prefix, 'config', 'rgbd.yaml')),
        DeclareLaunchArgument("rectify_rgb", default_value="True"),
    ]

    return LaunchDescription(
        declared_arguments + [OpaqueFunction(function=launch_camera)]
    )
