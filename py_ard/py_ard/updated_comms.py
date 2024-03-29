import rclpy
from rclpy.node import Node
import serial
from time import sleep
import struct
from std_msgs.msg import String
from geometry_msgs.msg import Twist
from sensor_msgs.msg import Joy
ser = serial.Serial('/dev/ttyACM0', baudrate=19200)
ser.reset_input_buffer()

ConveyorButton = 0
DeployButton = 0
DigLinButton = 0
DigBeltButton = 0


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
        global x, y, z
        ConveyorButton = msg.buttons[2]-msg.buttons[3]*255
        msgSend = msg.buttons[5]
        DeployButton = float(msg.buttons[6]-msg.buttons[7])*200
        DigLinButton = float(-1*(msg.axes[2])*255) #int(abs(msg.axes[2]-1)*255/2)
        DigBeltButton = float(abs(msg.axes[5]-1)*255)
        msgSend = msg.buttons[8]
        ser.reset_input_buffer()
        ser.reset_output_buffer()

        ser.write(String('<'))
        ser.write(String(float(x)))
        ser.write(String(','))
        ser.write(String(float(y)))
        ser.write(String(','))
        ser.write(String(float(z)))
        ser.write(String(','))
        ser.write(String(float(ConveyorButton)))
        ser.write(String(','))
        ser.write(String(float(DeployButton)))
        ser.write(String(','))
        ser.write(String(float(DigLinButton)))
        ser.write(String(','))
        ser.write(String(float(DigBeltButton)))
        ser.write(String('>'))



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
