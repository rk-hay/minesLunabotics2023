import rclpy
from rclpy.node import Node
import serial
from time import sleep
import struct
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
from nav2_simple_commander.robot_navigator import BasicNavigator

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
from tf_transformations import quaternion_from_euler

#nav.waitUntilNav2Active() 

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
    nav = BasicNavigator()
    state = 0 #if state is 0 -> rotate in place to find tag
    #Serial start
    ser = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=0) #TODO rules to change port name to MEGA
    ser.reset_input_buffer()


    def __init__(self):
        super().__init__('auto_node')
        self.waypoints = []  # List to store waypoints
        self.current_waypoint_index = 0  # Index of current waypoint
        self.waypoint_tolerance = 0.1  # Tolerance for considering waypoint as reached

        #Publishers
        #self.goal_publisher = self.create_publisher(PoseStamped, '/goal_pose', 10)

        #Subscribers
        self.odom_sub = self.create_subscription(Odometry, 'odom', self.odom_callback, 10)#TODO change odom call back to globalOdom
        self.start_digging_sub = self.create_subscription(Bool, 'start_digging', self.start_digging_callback, 10)
        self.subscription = self.create_subscription( Twist, 'cmd_vel', self.listener_callback, 10)
        #TODO ADD CAMERA SUBSCRIBER
        
    def publish_goal_pose(self, x, y, z, yaw, pitch, roll):
        #IF CAMERA NOT FACING FORWARD STOP MAKE FACE FORWARD, THEN GO
        msg = PoseStamped()
        msg.header.stamp.sec = 0
        msg.header.stamp.nanosec = 0
        msg.header.frame_id = "map"
        msg.pose.position.x = x
        msg.pose.position.y = y
        msg.pose.position.z = z
        quaternion = quaternion_from_euler(roll, pitch, yaw)
        msg.pose.orientation.x = quaternion[0]
        msg.pose.orientation.y = quaternion[1]
        msg.pose.orientation.z = quaternion[2]
        msg.pose.orientation.w = quaternion[3]
        self.nav.goToPose(msg)
        #self.goal_publisher.publish(msg)

    def check_waypoint_reached(self):
        """Check if the current waypoint is reached."""
        if not self.nav.isTaskComplete():
            feedback = self.nav.getFeedback()
            if feedback.navigation_duration > 6000: #TODO CHANGE TO REASONABLE AMOUNT OF TIME
                self.nav.cancelTask()
        else:
            result = self.nav.getResult()
            if result == self.nav.TaskResult.SUCCEEDED: #TODO IS THIS THE RIGHT SYNTAX FOR TASK GOOD?
                print('Goal succeeded!')
            elif result == self.nav.TaskResult.CANCELED:
                print('Goal was canceled!')
            elif result == self.nav.TaskResult.FAILED:
                print('Goal failed!')

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
        #turn off cmd_vel sending stuff
        #slide_out_until_stopped()
        #fold_out()
        #turn_on_bucket_conveyor()
        #start_timer_1()
        #start_slow_plunge()
        #turn_on_live_conveyor()
        #start_timer_2()

        #while not at_p3.y():
        #    if timer_1_hits(val):
        #        stop_for_x_seconds()
        #        reset_timer_1()
        #    if timer_2_hits(val2):
        #        reverse_plunge()
        #        reset_timer_1()

        #exit_dig_mode()
        #set_x_speed_high() go forward a smidge 

        #while not close_to_p4.y():
        #    if at_p4():
        #        set_x_speed_high_for_seconds()
        #    if at_p5():
        #        // Perform specific actions for p5


    def stateMachine(self):
        print(self.state)
        #states
        #0 -> map until marker found #TODO FIX MARKERS
        #1 -> publish goal
        #2 -> loop goal reached?? ->branch after x seconds to dif options
        #3 -> publish line up
        #4 -> loop goal reached?? ->branch close enough (fix using uwb??)
        #5 -> dig cycle

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
