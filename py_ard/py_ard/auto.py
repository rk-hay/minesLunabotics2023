import rclpy
from rclpy.node import Node
import serial
from time import sleep
import struct
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy

liveTrailer = 0
foldOut = 0
plunge = 0
buckeyConveyor = 0
slidOut = 0

x = 0
y = 0
z= 0

#Run this on LAPTOP

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
from nav_msgs.msg import Odometry
from std_msgs.msg import Bool

class AutoNode(Node):
    def __init__(self):
        super().__init__('auto_node')
        self.waypoints = []  # List to store waypoints
        self.current_waypoint_index = 0  # Index of current waypoint
        self.waypoint_tolerance = 0.1  # Tolerance for considering waypoint as reached
        self.reached_waypoint_pub = self.create_publisher(Bool, 'reached_waypoint', 10)
        self.odom_sub = self.create_subscription(Odometry, 'odom', self.odom_callback, 10)
        self.start_digging_sub = self.create_subscription(Bool, 'start_digging', self.start_digging_callback, 10)

    def set_waypoints(self, waypoints):
        """Set waypoints for the robot."""
        self.waypoints = waypoints

    def check_waypoint_reached(self):
        """Check if the current waypoint is reached."""
        current_pose = self.current_pose  # Get current pose from odometry
        target_pose = self.waypoints[self.current_waypoint_index]

        # Check if the distance between current pose and target pose is within tolerance
        distance = ((current_pose.position.x - target_pose.pose.position.x) ** 2 +
                    (current_pose.position.y - target_pose.pose.position.y) ** 2) ** 0.5
        if distance <= self.waypoint_tolerance:
            return True
        else:
            return False

    def start_digging_cycle(self):
        """Start the digging cycle."""
        # Implement your digging logic here
        pass

    def odom_callback(self, msg):
        """Callback function for odometry subscriber."""
        self.current_pose = msg.pose.pose

        # Check if the current waypoint is reached
        if self.check_waypoint_reached():
            # Publish that the waypoint is reached
            msg = Bool()
            msg.data = True
            self.reached_waypoint_pub.publish(msg)

            # Move to the next waypoint
            self.current_waypoint_index += 1
            if self.current_waypoint_index >= len(self.waypoints):
                self.start_digging_cycle()

    def start_digging_callback(self, msg):
        """Callback function for starting digging cycle."""
        if msg.data:
            self.start_digging_cycle()

def main(args=None):
    rclpy.init(args=args)
    auto_node = AutoNode()
    
    # Set waypoints
    waypoints = [...]  # Define your waypoints here
    auto_node.set_waypoints(waypoints)

    rclpy.spin(auto_node)

    auto_node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()



if __name__ == '__main__':
    main()
