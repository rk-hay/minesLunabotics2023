import rclpy
from rclpy.node import Node
import serial
from time import sleep
import struct
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
from nav2_simple_commander.robot_navigator import BasicNavigator
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped
from nav_msgs.msg import Odometry
from std_msgs.msg import Bool
from tf_transformations import quaternion_from_euler
import time, threading

#Run this on pi

#nav.waitUntilNav2Active() 

class AutoNode(Node):
    #PWM Values To Send/enables
    ConveyorButton = 0
    DeployButton = 0
    DigLinButton = 0
    DigBeltButton = 0
    BucketConveyor = 0
    digActivate = 0
    state = 'markerFound'

    #Velocities
    cmd_x = 0
    cmd_y = 0
    cmd_z = 0
    my_x = 0
    my_y = 0
    my_z = 0 
    x = 0
    y = 0
    z = 0
    cmd_vel_from_cmd_vel = True
    #MISC
    firstTime = True
    nav = BasicNavigator()

    #CONSTANTS
    xBerm = 6.10
    yBerm = 1.43
    lengthLARI = 1.44

    #Serial start
    ser = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=0) #TODO rules to change port name to MEGA
    ser.reset_input_buffer()


    def __init__(self):
        super().__init__('auto_node')
        #Publishers
        #self.goal_publisher = self.create_publisher(PoseStamped, '/goal_pose', 10)

        #Subscribers
        self.globalOdom_sub = self.create_subscription(Odometry, '/globalOdom', self.globalOdom_callback, 10) #TODO change odom call back to globalOdom
        self.Odom_sub = self.create_subscription(Odometry, '/Odom', self.Odom_callback, 10)
        self.subscription = self.create_subscription( Twist, '/cmd_vel', self.listener_callback, 10)
        self.SerialWrite()
        self.stateMachine()

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
            return 1
        else:
            result = self.nav.getResult()
            if result == self.nav.TaskResult.SUCCEEDED: #TODO IS THIS THE RIGHT SYNTAX FOR TASK GOOD?
                print('Goal succeeded!')
                return 2
            elif result == self.nav.TaskResult.CANCELED:
                print('Goal was canceled!')
                return -1
            elif result == self.nav.TaskResult.FAILED:
                print('Goal failed!')
                return -1


    def globalOdom_callback(self, msg):
        """Callback function for odometry subscriber."""
        self.UWB_pose = msg.pose.pose

    def Odom_callback(self, msg):
        """Callback function for odometry subscriber."""
        self.CAM_pose = msg.pose.pose
    
    def navigate_to_p1():
        print("write NAV function")
        #while not see_markers():
        #    rotate_slowly()
        #identify_markers()
        #line_up_approach()


    def dig_cycle(self):
        print("Write Dig Cycle")
        #ConveyorButton = 0
        #DeployButton = 0
        #DigLinButton = 0
        #DigBeltButton = 0
        #BucketConveyor = 0
        #digActivate = 0
        
        #turn off cmd_vel sending stuff
        #slide_out_until_stopped
        self.digActivate = 1
        self.x = 0
        sleep(15) #TODO adjust time or add feedback from PI
        self.DigBeltButton = 254
        self.BucketConveyor = 254
        currTime = time.time()
        plunge = True
        #going one way!
        while(abs(self.UWB_pose.postion.x-.5+self.xBerm) > .1): # repeat while front not to close to the dump zone
            while abs(self.UWB_pose.postion.y - 1.5+self.yBerm) > .2: #if we are on one side of the berm start diggin
                self.digActivate = 1
                self.DigBeltButton = 254 
                self.BucketConveyor = 254
                while time.time() - currTime < 45: #TODO change to more accurate timing?
                    if plunge == True:
                        self.DigLinButton = 179
                        plunge = False
                    else:
                        self.DigLinButton = 0
                        plunge = True
                    time.sleep(3)  # Wait for 3 seconds between state changes
                self.DigLinButton = -254
                self.DigBeltButton = 0 
                self.BucketConveyor = 0
                time.sleep(15) #TODO TIME
                self.x = .8
                time.sleep(1)
                self.x = 0.0
                currTime = time.time()
            ##exit_dig_mode
            self.digActivate = 0
            #ADD CHECK TO MAKE SURE PERPENDICULAR TO DIG ZONE
            
            #go forward a smidge 
            time.sleep(1)
            self.x = .8
            time.sleep(1)
            self.x = 0
            while abs(self.UWB_pose.postion.y - self.yBerm) > .2:
                self.digActivate = 1
                self.DigBeltButton = 254 
                self.BucketConveyor = 254
                while time.time() - currTime < 45: #TODO change to more accurate timing?
                    if plunge == True:
                        self.DigLinButton = 179
                        plunge = False
                    else:
                        self.DigLinButton = 0
                        plunge = True
                    time.sleep(5)  # Wait for 5 seconds between state changes
                self.DigLinButton = -254
                self.DigBeltButton = 0 
                self.BucketConveyor = 0
                time.sleep(15) #TODO TIME
                self.x = -.8
                time.sleep(1)
                self.x = 0.0
                currTime = time.time()
            self.digActivate = 0
            #ADD CHECK TO MAKE SURE PERPENDICULAR TO DIG ZONE
            
            #go forward a smidge 
            time.sleep(1)
            self.x = .8
            time.sleep(1)
            self.x = 0
        self.state = 'IDK'

    def stateMachine(self):
        print(self.state)
        #states

        #0 -> map until marker found 
            #goto target orientation
        if (state == 'markerFound'):
            while (abs(1.57 - self.UWB_pose.orientation.z) > .78): #TODO THIS WHILE LOOP MIGHT NEED TO BE NONBLOCKING
                prevOrient = self.UWB_pose.orientation.z
                self.cmd_vel_from_cmd_vel = False
                self.z = .5
                self.SerialWrite()
                sleep(1.5)
                self.z = 0
                self.SerialWrite()
                while(abs(self.UWB_pose.orientation.z-prevOrient) > .01):
                    #TODO WATCHDOG
                    print("Waiting for orientation update")
            state = 'gotoDig'
        #1 -> publish goal
        elif(state == 'gotoDig'):
            self.publish_goal_pose(self.xBerm, self.yBerm+self.lengthLARI, self.CAM_pose.position.z, 1.57, 0.0, 0.0): #TODO UPDATE FOR KENNEDY #FOR UCF X=6.10 (close side of berm) Y=1.43 Lari is 1.44 M long
            self.cmd_vel_from_cmd_vel = True 
            while self.check_waypoint_reached() != 2:
                #state = 'linedUP' #add
                self.cmd_vel_from_cmd_vel = False
                state = 'digCycle'
        #->branch after x seconds to dif options
        #3 -> publish line up
        #4 -> loop goal reached?? ->branch close enough (fix using uwb??)
        #5 -> dig cycle
        elif(state == 'digCycle'):
            self.dig_cycle()
        elif(state == 'IDK'):
            print("AT END")
            self.x = 0
            self.y = 0
            self.z = 0

        if self.cmd_vel_from_cmd_vel == True:
            self.x = self.cmd_x
            self.y = self.cmd_y
            self.z = self.cmd_z
        else:
            self.x = self.my_x
            self.y = self.my_y
            self.z = self.my_z
        

    def SerialWrite(self):
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
        threading.Timer(.1, self.SerialWrite).start()


    def cmd_vel_listener(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.linear.x)
        self.cmd_x = msg.linear.x
        self.cmd_y = msg.linear.y
        self.cmd_z = msg.angular.z



def main(args=None):
    rclpy.init(args=args)
    auto_node = AutoNode()

    rclpy.spin(auto_node)

    auto_node.destroy_node()
    rclpy.shutdown()

    #navigate_to_p1()
    #dig_cycle()

if __name__ == '__main__':
    main()


