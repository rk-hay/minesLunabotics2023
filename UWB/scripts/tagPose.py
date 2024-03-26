#!/usr/bin/python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped 
from nav_msgs.msg import Odometry
import socket
import time

hostname = socket.gethostname()
UDP_IP =  socket.gethostbyname(hostname)
print("***Local ip:" + '192.168.107.8' + "***")
UDP_PORT = 5000
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind(('192.168.107.8', UDP_PORT))
sock.listen(1) 
data, addr = sock.accept()

class tagPublisher(Node):
    x = 0
    y = 0
    e = 0
    def __init__(self):
        super().__init__('tagPublisher')
        self.publisher_ = self.create_publisher(Odometry , 'Odometry', 10)
        self.timer = self.create_timer(.1, self.publish_PoseStamped)

    def publish_PoseStamped(self):
        self.read_data()
        odom = Odometry()
        odom.header.frame_id = "odom"
        odom.header.stamp = self.get_clock().now().to_msg()
        odom.pose.pose.position.x = self.x
        odom.pose.pose.position.y = self.y
        odom.pose.pose.position.z = 0.0
        odom.pose.pose.orientation.x = 0.0#quat_x
        odom.pose.pose.orientation.y = 0.0#quat_y
        odom.pose.pose.orientation.z = 0.0#quat_z FIGURE OUT HOW TO ADD
        odom.pose.pose.orientation.w = 0.0#quat_w
        odom.child_frame_id = "base_link"
        odom.twist.twist.linear.x
        odom.twist.twist.linear.y = 0.0
        odom.twist.twist.linear.z = 0.0
        self.publisher_.publish(odom)

    def read_data(self):
        try:
            data.settimeout(5)  # Set a timeout of 5 seconds
            received_data = data.recv(1024).decode('utf-8')
            data.settimeout(None)  # Reset the timeout

            try:
                print(received_data)
                # Split the received data using comma as delimiter
                parts = received_data.split(',')

                # Ensure all parts are present before assigning values
                if len(parts) == 3:
                    self.x = float(parts[0])
                    self.y = float(parts[1])
                    self.e = float(parts[2])
                else:
                    print("Incomplete data received:", received_data)
            except:
                print("Error:")
                print("Failed to parse received data:", received_data)
            print("")
            return received_data
        except socket.timeout:
            print("Socket timeout. No data received.")
            data.settimeout(None)
            return 0


def main(args=None):
    rclpy.init(args=args)
    node = tagPublisher()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
