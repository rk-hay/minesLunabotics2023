import rclpy
from rclpy.node import Node
import serial
from time import sleep
import struct
from std_msgs.msg import String
from geometry_msgs.msg import Twist

ser = serial.Serial('/dev/ttyACM0', baudrate=115200)
ser.reset_input_buffer()
prevX = 0
prevY = 0
prevZ = 0
class MinimalSubscriber(Node):
    

    def __init__(self):
        super().__init__('minimal_subscriber')
        
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        #self.get_logger().info('I heard: "%s"' % msg.linear.x)
        global prevX, prevY, prevZ
        # ser.reset_input_buffer()
        
        if abs(msg.linear.x) - abs(prevX) > .05 or abs(msg.linear.y) - abs(prevY) > .02 or abs(msg.angular.z) - abs(prevZ) > .5 :
            #ser.reset_input_buffer
            #ser.reset_output_buffer
            ser.write(struct.pack('c', b'L'))
            ser.write(struct.pack('c', b'X'))
            ser.write(struct.pack('<f', float(msg.linear.x)))
            #ser.write(struct.pack('c', b'\n'))
            ser.write(struct.pack('c', b'L'))
            ser.write(struct.pack('c', b'Y'))
            ser.write(struct.pack('<f', float(msg.linear.y)))
            #ser.write(struct.pack('c', b'\n'))
            ser.write(struct.pack('c', b'A'))
            ser.write(struct.pack('c', b'Z'))
            ser.write(struct.pack('<f', float(msg.angular.z)))
            #ser.write(struct.pack('c', b'\n'))
            #ser.write(struct.pack('c', b'S'))
            self.get_logger().info('test\n')
            #self.get_logger().info(ser.readline())
            prevX = msg.linear.x
            prevY = msg.linear.y
            prevZ = msg.angular.z
            sleep(0.1)
            
            


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = MinimalSubscriber()
    #minimal_subscriber.create_rate(10)
    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    ser.close()
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()