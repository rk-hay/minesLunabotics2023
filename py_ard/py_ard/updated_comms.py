import rclpy
from rclpy.node import Node
import serial
from time import sleep
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
from nav_msgs.msg import Odometry

import time, threading

ser = serial.Serial('/dev/ttyACM0', baudrate=115200, timeout=0)
ser.reset_input_buffer()



class VelocityComm(Node):
    cmd_x = 0
    cmd_y = 0
    cmd_z = 0

    ConveyorButton = 0
    DeployButton = 0
    DigLinButton = 0
    DigBeltButton = 0
    BucketConveyor = 0
    digActivate = 0

    _ConveyorButton = 0
    _DeployButton = 0
    _DigLinButton = 0
    _DigBeltButton = 0
    _BucketConveyor = 0
    _digActivate = 0

    firstTime = True

    x = 0
    y = 0
    z= 0

    x_offset = -.877
    y_offset = .296

    cmd_vel_from_cmd_vel = True
    dig = 0
    prevDig = 0

    xBerm = 6.10
    yBerm = 4.57-1.43
    lengthLARI = 1.44

    UWB_pose = Odometry()

    ArenaLength = 8.14
    ArenaHeight = 4.57
    def __init__(self):
        super().__init__('Comm')
        self.subscription2 = self.create_subscription(
            Joy,
            'joy',
            self.CallMe,
            10)
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.listener_callback,
            10)
        self.globalOdom_sub = self.create_subscription(Odometry, '/globalOdom', self.globalOdom_callback, 10) #TODO change odom call back to globalOdom
        self.comms()

        self.subscription
        self.subscription2   
    def listener_callback(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.linear.x)
        self.cmd_x = msg.linear.x
        self.cmd_y = msg.linear.y
        self.cmd_z = msg.angular.z
            

    def globalOdom_callback(self, msg):
        """Callback function for odometry subscriber."""
        self.UWB_pose = msg


    def CallMe(self, msg):
        self._ConveyorButton = (msg.buttons[2]-msg.buttons[3])*255
        self._DeployButton = float(msg.buttons[6]-msg.buttons[7])*255
        self._DigLinButton = float(-1*(msg.axes[2])*255) #int(abs(msg.axes[2]-1)*255/2)
        self._DigBeltButton = float(abs(msg.axes[5]-1)*255/2)
        self._BucketConveyor = float(msg.buttons[5])*255
        self._digActivate = float(msg.buttons[4])
        self.dig = msg.buttons[8]
        if self.dig and self.dig != self.prevDig:
            self.dig_cycle()
            self.prevDig = self.dig

    def comms(self):
        if self.firstTime == True:
            self.get_logger().info('waiting for arduino')
            sleep(5)
            self.get_logger().info('arduino ready')
            self.firstTime = False  

        if self.cmd_vel_from_cmd_vel:
            self.x = self.cmd_x
            self.y = self.cmd_y
            self.z = self.cmd_z
            self.ConveyorButton = self._ConveyorButton
            self.DeployButton = self._DeployButton
            self.DigLinButton = self._DigLinButton
            self.DigBeltButton = self._DigBeltButton
            self.BucketConveyor = self._BucketConveyor
            self.digActivate = self._digActivate
            


        ser.reset_input_buffer()
        ser.reset_output_buffer()
        start = "<"
        finish = str(">")
        ser.write(start.encode())
        ser.write(str(float(self.x)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.y)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.z)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.digActivate)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.ConveyorButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.DeployButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.DigLinButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.DigBeltButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(self.BucketConveyor)).encode())
        ser.write(finish.encode())
        #self.get_logger().info('I heard: "%s"' % self.digActivate)
        
        threading.Timer(.1, self.comms).start()


    def dig_cycle(self):
        print("Dig Cycle")
        self.cmd_vel_from_cmd_vel = False

        self.digActivate = 1 #wheels go 90
        self.ConveyorButton = -254 #slide out moves forward
        self.x = 0 # speed is 0
        self.y = 0
        self.z = 0
        sleep(20) #TODO adjust time or add feedback from PI
        self.ConveyorButton = 0 #stop the slide outs
        self.DeployButton = 254
        sleep(7)
        self.DeployButton = 0
        sleep(5)
        self.DigBeltButton = 254
        #self.BucketConveyor = 254
        currTime = time.time()
        startTime = time.time()
        plunge = True
        #going one way!
        #xBerm = 6.10
        #yBerm = 3.14
        #x_offset = -.877
        #y_offset = .296
        
        while(abs(self.UWB_pose.pose.pose.position.y-(.59+self.yBerm)) > .1): # repeat while front not to close to the dump zone
        #while(time.time()-startTime < 140):

            self.get_logger().info('distance stop Y postion : "%s"' % abs(self.UWB_pose.pose.pose.position.y-(.5+self.yBerm)))
            while abs(self.UWB_pose.pose.pose.position.x - (1.5+self.xBerm)) > .2: #if we are on one side of the berm start diggin
                self.get_logger().info('distance stop X postion : "%s"' % abs(self.UWB_pose.pose.pose.position.x - (1.5+self.xBerm)))    
            #while(time.time()-startTime < 60):
    
                self.digActivate = 1
                self.DigBeltButton = 254 
                currTime = time.time()-1
                while time.time() - currTime < 20: #TODO change to more accurate timing?
                    if plunge == True:
                        self.get_logger().info('Plunge On')
                        self.DigLinButton = 179
                        self.BucketConveyor = 254
                        plunge = False
                    else:
                        self.get_logger().info('Plunge Off')
                        self.DigLinButton = 0
                        self.BucketConveyor = 0
                        plunge = True
                    time.sleep(3)  # Wait for 3 seconds between state changes
                self.DigLinButton = -254
                self.DigBeltButton = 0 
                self.BucketConveyor = 0
                time.sleep(15) #TODO TIME to retract plunger
                self.x = .8
                time.sleep(1)
                self.x = 0.0
                
            ##exit_dig_mode
            self.digActivate = 0
            #ADD CHECK TO MAKE SURE PERPENDICULAR TO DIG ZONE
            
            #go forward a smidge 
            time.sleep(2)
            self.x = .8
            time.sleep(1)
            self.x = 0
            self.DigBeltButton = 0


            while abs(self.UWB_pose.pose.pose.position.x - self.xBerm) > .2:
            #while(time.time()-startTime < 120):
    
                self.digActivate = 1
                self.DigBeltButton = 254 
                currTime = time.time()
                while time.time() - currTime < 20: #TODO change to more accurate timing?
                    if plunge == True:
                        self.DigLinButton = 179
                        self.BucketConveyor = 254
                        plunge = False
                    else:
                        self.DigLinButton = 0
                        self.BucketConveyor = 0
                        plunge = True
                    time.sleep(3)  # Wait for 3 seconds between state changes
                self.DigLinButton = -254
                self.DigBeltButton = 0 
                self.BucketConveyor = 0
                time.sleep(15) #TODO TIME to retract plunger
                self.x = -.8
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

def main(args=None):
    rclpy.init(args=args)

    velocityComm = VelocityComm()
    #buttonPressComm = ButtonPressComm()
    #minimal_subscriber.create_rate(10)
    rclpy.spin(velocityComm)
    #rclpy.spin(buttonPressComm)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    ser.close()
    velocityComm.destroy_node()
    #buttonPressComm.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
