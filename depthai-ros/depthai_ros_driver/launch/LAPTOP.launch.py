import os

from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import DeclareLaunchArgument, IncludeLaunchDescription, OpaqueFunction
from launch.conditions import IfCondition
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch.substitutions import LaunchConfiguration
from launch_ros.actions import LoadComposableNodes, Node
from launch_ros.descriptions import ComposableNode

def launch_setup(context, *args, **kwargs):
    name = LaunchConfiguration('name').perform(context)
    depthai_prefix = get_package_share_directory("depthai_ros_driver")

    params_file= LaunchConfiguration("params_file")
    parameters = [
        {
            "frame_id": 'base_link',
            "subscribe_rgb": True,
            "subscribe_depth": True,
            "subscribe_odom_info": True,
            "approx_sync": True,
            "compressed":True,
            "Rtabmap/DetectionRate": "3.5",
            "rtabmap_args:=": "--delete_db_on_start",
            "Grid/MaxGroundAngle": "60",
            "Grid/CellSize": "0.1",
            "Grid/ClusterRadius": "0.2",
            "Grid/MaxGroundHeight": ".1",
            "Grid/MaxObstacleHeight": "1.0",
            "Grid/NoiseFilteringRadius": "1.0",
            "RGBD/MarkerDetection": "True",
            "Marker/Dictionary": "20", 
            "Marker/Priors": "9 1 4.58 .34 0 0 -1.57|10 7.14 4.58 .34 0 0 -1.57",
            "Marker/VarianceAngular": ".01",
            #Odometry: initial_pose
            #"Optimizer/PriorsIgnored": "False",
            #"cloud_subtract_filtering": True,
            #"cloud_subtract_filtering_min_neighbors": "6",
            #"reateOccupancyGrid" : True,
            #"odom_frame_id": 'globalOdom',
        }
    ]

    remappings = [
        ("rgb/image", name+"/rgb/image_rect"),
        ("rgb/camera_info", name+"/rgb/camera_info"),
        ("depth/image", name+"/stereo/image_raw"),
        ("/map", "/arena"),
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

        Node(
            package="rtabmap_viz",
            executable="rtabmap_viz",
            output="screen",
            parameters=parameters,
            remappings=remappings,
        ),

        Node(
            package="tf2_ros",
            executable="static_transform_publisher",
            name="oak_static_transform_publisher",
            output="screen",
            arguments=["0", "0", ".70", "3.16", "0", "0", "base_link", "oak"]
        ),
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
