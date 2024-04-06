import rclpy
from rclpy.node import Node
import serial
from time import sleep
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
ser = serial.Serial('/dev/ttyACM0', baudrate=19200, timeout=0)
ser.reset_input_buffer()

ConveyorButton = 0
DeployButton = 0
DigLinButton = 0
DigBeltButton = 0
BucketConveyor = 0
digActivate = 0
firstTime = True

x = 0
y = 0
z= 0

class VelocityComm(Node):
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

        self.subscription
        self.subscription2   
    def listener_callback(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.linear.x)
        global ConveyorButton 
        global DeployButton
        global DigLinButton
        global DigBeltButton
        global msgSend
        global x, y, z
        x = msg.linear.x
        y = msg.linear.y
        z = msg.angular.z
            

            


    def CallMe(self, msg):
        global ConveyorButton 
        global DeployButton
        global DigLinButton
        global DigBeltButton
        global msgSend
        global BucketConveyor
        global digActivate
        global x, y, z
        global firstTime
        if firstTime == True:
            self.get_logger().info('waiting for arduino')
            sleep(5)
            firstTime = False  
            
        ConveyorButton = (msg.buttons[2]-msg.buttons[3])*255
        DeployButton = float(msg.buttons[6]-msg.buttons[7])*255
        DigLinButton = float(-1*(msg.axes[2])*255) #int(abs(msg.axes[2]-1)*255/2)
        DigBeltButton = float(abs(msg.axes[5]-1)*255/2)
        BucketConveyor = float(msg.buttons[5])*255
        digActivate = float(msg.buttons[4])
        
        msgSend = msg.buttons[8]
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        start = "<"
        finish = str(">")
        ser.write(start.encode())
        ser.write(str(float(x)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(y)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(z)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(digActivate)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(ConveyorButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(DeployButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(DigLinButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(DigBeltButton)).encode())
        ser.write(str(',').encode())
        ser.write(str(float(BucketConveyor)).encode())
        ser.write(finish.encode())
        self.get_logger().info('I heard: "%s"' % digActivate)
        
        sleep(0.1)
	

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
