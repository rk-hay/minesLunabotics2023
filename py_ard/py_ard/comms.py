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
        self.subscription2  # prevent unused variable warning

    def listener_callback(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.linear.x)
        global prevX, prevY, prevZ
        global ConveyorButton, prevConveyorButton 
        global DeployButton, prevDeployButton
        global DigLinButton, prevDigLinButton
        global DigBeltButton, prevDigBeltButton
        global msgSend, prevMsgSend
        global x, y, z
        x = msg.linear.x
        y = msg.linear.y
        z = msg.angular.z
            

            


    def CallMe(self, msg):
        global prevX, prevY, prevZ
        global ConveyorButton, prevConveyorButton 
        global DeployButton, prevDeployButton
        global DigLinButton, prevDigLinButton
        global DigBeltButton, prevDigBeltButton
        global msgSend, prevMsgSend
        global x, y, z
        ConveyorButton = (msg.buttons[2]-msg.buttons[3])*255
        msgSend = msg.buttons[5]
        DeployButton = float(msg.buttons[6]-msg.buttons[7])*200
        DigLinButton = float(-1*(msg.axes[2])*255) #int(abs(msg.axes[2]-1)*255/2)
        DigBeltButton = float(abs(msg.axes[5]-1)*255)
        msgSend = msg.buttons[8]
        if abs(x - prevX) > .02 or abs(y - prevY) > .02 or abs(z - prevZ) > .02 or prevConveyorButton != ConveyorButton or prevDeployButton != DeployButton or prevDigLinButton != DigLinButton or prevDigBeltButton != DigBeltButton or msgSend !=prevMsgSend:
            ser.reset_input_buffer()
            ser.reset_output_buffer()

            ser.write(struct.pack('c', b'X'))
            ser.write(struct.pack('<f', float(x)))
            ser.write(struct.pack('c', b'\n'))
            ser.write(struct.pack('c', b'Y'))
            ser.write(struct.pack('<f', float(y)))
            ser.write(struct.pack('c', b'\n'))
            ser.write(struct.pack('c', b'Z'))
            ser.write(struct.pack('<f', float(z)))
            ser.write(struct.pack('c', b'\n'))

            ser.write(struct.pack('c', b'A'))
            ser.write(struct.pack('<f', float(ConveyorButton)))
            ser.write(struct.pack('c', b'\n'))

            ser.write(struct.pack('c', b'B'))
            ser.write(struct.pack('<f', float(DeployButton)))
            ser.write(struct.pack('c', b'\n'))
            
            ser.write(struct.pack('c', b'C'))
            ser.write(struct.pack('<f', float(DigLinButton)))
            ser.write(struct.pack('c', b'\n'))

            ser.write(struct.pack('c', b'D'))
            ser.write(struct.pack('<f', float(DigBeltButton)))
            ser.write(struct.pack('c', b'\n'))

            prevX = x
            prevY = y
            prevZ = z
            self.get_logger().info('I heard: "%s"' % prevDeployButton)
            self.get_logger().info('\n I heard: "%s"' % DeployButton)
            prevMsgSend = msgSend
            prevConveyorButton = ConveyorButton
            prevDeployButton = DeployButton
            prevDigLinButton = DigLinButton
            prevDigBeltButton = DigBeltButton       
            self.get_logger().info('test\n')
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
