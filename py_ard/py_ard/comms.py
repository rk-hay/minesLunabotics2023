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
prevX = 0
prevY = 0
prevZ = 0

ConveyorButton = 0
DeployButton = 0
DigLinButton = 0
DigBeltButton = 0

prevConveyorButton = 0
prevDeployButton = 0
prevDigLinButton = 0
prevDigBeltButton = 0
msgSend = 0
prevMsgSend = 0

class VelocityComm(Node):
    def __init__(self):
        super().__init__('VelocityComm')
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.linear.x)
        global prevX, prevY, prevZ
        global ConveyorButton, prevConveyorButton 
        global DeployButton, prevDeployButton
        global DigLinButton, prevDigLinButton
        global DigBeltButton, prevDigBeltButton
        global msgSend, prevMsgSend
        
        if abs(msg.linear.x - prevX) > .02 or abs(msg.linear.y - prevY) > .02 or abs(msg.angular.z - prevZ) > .02 or prevConveyorButton != ConveyorButton or prevDeployButton != DeployButton or prevDigLinButton != DigLinButton or prevDigBeltButton != DigBeltButton or msgSend !=prevMsgSend:
            ser.reset_input_buffer()
            ser.reset_output_buffer()

            ser.write(struct.pack('c', b'X'))
            ser.write(struct.pack('<f', float(msg.linear.x)))
            ser.write(struct.pack('c', b'\n'))
            ser.write(struct.pack('c', b'Y'))
            ser.write(struct.pack('<f', float(msg.linear.y)))
            ser.write(struct.pack('c', b'\n'))
            ser.write(struct.pack('c', b'Z'))
            ser.write(struct.pack('<f', float(msg.angular.z)))
            ser.write(struct.pack('c', b'\n'))

            ser.write(struct.pack('c', b'A'))
            ser.write(struct.pack('<i', int(ConveyorButton)))
            ser.write(struct.pack('c', b'\n'))

            ser.write(struct.pack('c', b'B'))
            ser.write(struct.pack('<i', int(DeployButton)))
            ser.write(struct.pack('c', b'\n'))
            
            ser.write(struct.pack('c', b'C'))
            ser.write(struct.pack('<i', int(DigLinButton)))
            ser.write(struct.pack('c', b'\n'))

            ser.write(struct.pack('c', b'D'))
            ser.write(struct.pack('<i', int(DigBeltButton)))
            ser.write(struct.pack('c', b'\n'))


            #ser.write(struct.pack('c', b'S'))
            self.get_logger().info('test\n')
            #self.get_logger().info(ser.readline())

            prevX = msg.linear.x
            prevY = msg.linear.y
            prevZ = msg.angular.z
            
            prevMsgSend = msgSend
            prevConveyorButton = ConveyorButton
            prevDeployButton = DeployButton
            prevDigLinButton = DigLinButton
            prevDigBeltButton = DigBeltButton

            sleep(0.1)
            
            
class ButtonPressComm(Node):
    def __init__(self):
        super().__init__('ButtonPressComm')
        self.subscription = self.create_subscription(
            Joy,
            'joy',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        global ConveyorButton
        global DeployButton
        global DigLinButton
        global DigBeltButton
        global msgSend
        
        ConveyorButton = msg.buttons[4]
        msgSend = msg.buttons[5]
        DeployButton = msg.axes[7]*200
        DigLinButton = int(-1*(msg.axes[2])*255) #int(abs(msg.axes[2]-1)*255/2)
        DigBeltButton = int(abs(msg.axes[5]-1)*255/2)
        msgSend = 



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
