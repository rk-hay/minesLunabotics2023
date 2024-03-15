from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LoadComposableNodes
from launch_ros.descriptions import ComposableNode
from ament_index_python.packages import get_package_share_directory
import os

def launch_setup(context):
    name = LaunchConfiguration('name').perform(context)
    depthai_prefix = get_package_share_directory("depthai_ros_driver")

    params_file= LaunchConfiguration("params_file")
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
        ("stereo/left/image_rect_color", name+"/left/image_rect_color"),  # Subscribing to left camera image
        ("stereo/left/camera_info", name+"/left/camera_info"),
        ("stereo/right/image_rect_color", name+"/right/image_rect_color"),  # Subscribing to right camera image
        ("stereo/right/camera_info", name+"/right/camera_info"),
        ("depth/image", name+"/stereo/image_raw/compressedDepth"),  # Subscribing to compressed depth image
    ]

    return [
        LoadComposableNodes(
            target_container=name+"_container",
            composable_node_descriptions=[
                ComposableNode(
                    package='rtabmap_odom',
                    plugin='rtabmap_odom::RGBDOdometry',
                    name='rgbd_odometry',
                    parameters=parameters,
                    remappings=remappings,
                ),
            ],
        ),

        LoadComposableNodes(
            target_container=name+"_container",
            composable_node_descriptions=[
                ComposableNode(
                    package='rtabmap_slam',
                    plugin='rtabmap_slam::CoreWrapper',
                    name='rtabmap',
                    parameters=parameters,
                    remappings=remappings,
                ),
            ],
        ),

        LoadComposableNodes(
            condition=IfCondition(LaunchConfiguration("rectify_rgb")),
            target_container=name+"_container",
            composable_node_descriptions=[
                ComposableNode(
                    package="image_proc",
                    plugin="image_proc::RectifyNode",
                    name="rectify_color_node",
                    remappings=[('image', name+'/rgb/image_raw'),
                                ('camera_info', name+'/rgb/camera_info'),
                                ('image_rect', name+'/rgb/image_rect'),
                                ('image_rect/compressed', name+'/rgb/image_rect/compressed'),
                                ('image_rect/compressedDepth', name+'/rgb/image_rect/compressedDepth'),
                                ('image_rect/theora', name+'/rgb/image_rect/theora')]
                )
            ]),
    ]

def generate_launch_description():
    depthai_prefix = get_package_share_directory("depthai_ros_driver")
    declared_arguments = [
        DeclareLaunchArgument("name", default_value="oak"),
        DeclareLaunchArgument("params_file", default_value=os.path.join(depthai_prefix, 'config', 'rgbd.yaml')),
        DeclareLaunchArgument("rectify_rgb", default_value="True"),
    ]

    return LaunchDescription(
        declared_arguments + [OpaqueFunction(function=launch_setup)]
    )
