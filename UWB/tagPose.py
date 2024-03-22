import rclpy
from rclpy.node import Node
from geometry_msgs.msg import PoseStamped 
import socket
import time

hostname = socket.gethostname()
UDP_IP =  socket.gethostbyname(hostname)
print("***Local ip:" + str(UDP_IP) + "***")
UDP_PORT = 80
sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((UDP_IP, UDP_PORT))
sock.listen(1) 
data, addr = sock.accept()

class tagPublisher(Node):
    x = 0
    y = 0
    e = 0
    def __init__(self):
        super().__init__('tagPublisher')
        self.publisher_ = self.create_publisher(PoseStamped , 'PoseStamped', 10)
        self.timer = self.create_timer(.1, self.publish_PoseStamped)

    def publish_PoseStamped(self):
        self.read_data()
        msg = PoseStamped()
        msg.header.stamp = self.get_clock().now().to_msg()
        msg.pose.position.x = self.x
        msg.pose.position.y = self.y
        msg.pose.position.z = 0
        msg.pose.orientation.x = 0#quat_x
        msg.pose.orientation.y = 0#quat_y
        msg.pose.orientation.z = 0#quat_z FIGURE OUT HOW TO ADD
        msg.pose.orientation.w = 0#quat_w
        self.publisher_.publish(msg)

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