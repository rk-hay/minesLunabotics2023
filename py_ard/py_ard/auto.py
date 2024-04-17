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

#Run this on pi

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
from nav_msgs.msg import Odometry
from std_msgs.msg import Bool

class AutoNode(Node):
    #PWM Values To Send/enables
    ConveyorButton = 0
    DeployButton = 0
    DigLinButton = 0
    DigBeltButton = 0
    BucketConveyor = 0
    digActivate = 0
    #Velocities
    x = 0
    y = 0
    z = 0
    #MISC
    firstTime = True

    #Serial start
    ser = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=0) #TODO rules to change port name to MEGA
    ser.reset_input_buffer()


    def __init__(self):
        super().__init__('auto_node')
        self.waypoints = []  # List to store waypoints
        self.current_waypoint_index = 0  # Index of current waypoint
        self.waypoint_tolerance = 0.1  # Tolerance for considering waypoint as reached

        #Publishers
        self.reached_waypoint_pub = self.create_publisher(Bool, 'reached_waypoint', 10)

        #Subscribers
        self.odom_sub = self.create_subscription(Odometry, 'odom', self.odom_callback, 10)#TODO change odom call back to globalOdom
        self.start_digging_sub = self.create_subscription(Bool, 'start_digging', self.start_digging_callback, 10)
        self.subscription = self.create_subscription( Twist, 'cmd_vel', self.listener_callback, 10)
        #TODO ADD CAMERA SUBSCRIBER
        
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
    
    def navigate_to_p1():
        print("write NAV function")
        #while not see_markers():
        #    rotate_slowly()
        #identify_markers()
        #line_up_approach()


    def dig_cycle():
        print("Write Dig Cycle")
        #slide_out_until_stopped()
        #fold_out()
        #turn_on_bucket_conveyor()
        #start_timer_1()
        #start_slow_plunge()
        #turn_on_live_conveyor()
        #start_timer_2()

        #while not at_p3():
        #    if timer_1_hits(val):
        #        stop_for_x_seconds()
        #        reset_timer_1()
        #    if timer_2_hits(val2):
        #        reverse_plunge()
        #        reset_timer_1()

        #exit_dig_mode()
        #set_x_speed_high()

        #while not close_to_p4():
        #    if at_p4():
        #        set_x_speed_high_for_seconds()
        #    if at_p5():
        #        // Perform specific actions for p5


    def SerialWrite(self, msg):
        #TODO MAKE SURE BUTTONS ARE CORRECTLY NAMED 
        if self.firstTime == True:
            self.get_logger().info('waiting for arduino')
            sleep(5)
            self.firstTime = False  
        
        self.ser.reset_input_buffer()
        self.ser.reset_output_buffer()
        start = "<"
        finish = str(">")
        self.ser.write(start.encode())
        self.ser.write(str(float(self.x)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.y)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.z)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.digActivate)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.ConveyorButton)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.DeployButton)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.DigLinButton)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.DigBeltButton)).encode())
        self.ser.write(str(',').encode())
        self.ser.write(str(float(self.BucketConveyor)).encode())
        self.ser.write(finish.encode())

        #TODO IF DEBUGGER THEN PRINT
        #self.get_logger().info('I heard: "%s"' % self.digActivate)
        #TODO MESS WITH SHORTER TIMES?
        sleep(0.1)


    def cmd_vel_listener(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.linear.x)
        self.x = msg.linear.x
        self.y = msg.linear.y
        self.z = msg.angular.z



def main(args=None):
    rclpy.init(args=args)
    auto_node = AutoNode()
    
    # Set waypoints
    waypoints = [...]  # Define your waypoints here
    auto_node.set_waypoints(waypoints)

    rclpy.spin(auto_node)

    auto_node.destroy_node()
    rclpy.shutdown()

    #navigate_to_p1()
    #dig_cycle()

if __name__ == '__main__':
    main()



if __name__ == '__main__':
    main()
