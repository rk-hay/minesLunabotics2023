import rclpy
from rclpy.node import Node
import serial
from time import sleep
import struct
from std_msgs.msg import String
from geometry_msgs.msg import Twist

ser = serial.Serial('/dev/ttyACM0', baudrate=115200)
ser.reset_input_buffer()
#prevX = 0
#prevY = 0
#prevZ = 0

class MinimalSubscriber(Node):
    

    def __init__(self):
        super().__init__('minimal_subscriber')
        
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.listener_callback,
            1)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        #global prevX 
        #global prevY 
        #global prevZ 
        # ser.reset_input_buffer()
        # self.get_logger().info('I heard: "%s"' % msg.linear.x)  #format msg.linear.x
        if msg.linear.x > 10 or msg.linear.x  < -10 or msg.linear.y > 10 or msg.linear.y  < -10 or msg.angular.z > 10 or msg.angular.z < -10:
            ser.write(struct.pack('c', b'S'))
            ser.write(struct.pack('<f', float(msg.linear.x)))
            ser.write(struct.pack('<f', float(msg.linear.y)))
            ser.write(struct.pack('<f', float(msg.angular.z)))
            #if ser.in_waiting > 0:
            #    ch = ser.readline().decode('ascii')
            #    self.get_logger().info('"%s"' % ch)
        #prevX = msg.linear.x
        #prevY = msg.linear.y
        #prevZ = msg.angular.z
            #sleep(0.1)
            
            


def main(args=None):
    rclpy.init(args=args)

    minimal_subscriber = MinimalSubscriber()
    minimal_subscriber.create_rate(10)
    rclpy.spin(minimal_subscriber)

    # Destroy the node explicitly
    # (optional - otherwise it will be done automatically
    # when the garbage collector destroys the node object)
    ser.close()
    minimal_subscriber.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()