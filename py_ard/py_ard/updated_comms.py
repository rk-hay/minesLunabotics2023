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
prevButton = 0
velSub = 'cmd_vel'

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
        #global prevX, prevY, prevZ
        # ser.reset_input_buffer()
        
        #if abs(msg.linear.x - prevX) > .05 or abs(msg.linear.y - prevY) > .02 or abs(msg.angular.z - prevZ) > .05:
            #ser.reset_input_buffer()
            #ser.reset_output_buffer
            #ser.reset_output_buffer
        x =msg.linear.x
        y =msg.linear.y
        z =msg.angular.z
        ser.write(struct.pack('c', b'X'))
        ser.write(x.encode())
        ser.write(struct.pack('c', b'\n'))
        ser.write(struct.pack('c', b'Y'))
        ser.write(y.encode())
        ser.write(struct.pack('c', b'\n'))
        ser.write(struct.pack('c', b'Z'))
        ser.write(z.encode())
        ser.write(struct.pack('c', b'\n'))
        #self.get_logger().info('test\n')
        #self.get_logger().info(ser.readline())
        #prevX = x
        #prevY = y
        #prevZ = z
        #sleep(0.1)
            
            




def main(args=None):
    rclpy.init(args=args)

    velocityComm = VelocityComm()
    #buttonPressComm = ButtonPressComm()
    #minimal_subscriber.create_rate(10)
    rclpy.spin(velocityComm)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    ser.close()
    velocityComm.destroy_node()
    #buttonPressComm.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
