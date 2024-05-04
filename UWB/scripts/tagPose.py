#!/usr/bin/python3
import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Quaternion
from nav_msgs.msg import Odometry
from tf_transformations import quaternion_from_euler
import math
import serial

class tagPublisher(Node):
    x_1 = 0.0
    y_1 = 0.0
    x_2 = 0.0
    y_2 = 0.0
    dx = 0.0
    dy = 0.0
    prevX = 0.0
    prevY = 0.0
    prevTime = 0.0
    yaw = 0.0
    mag = 0.0
    e = 0.0
    port1 = '/dev/ttyUSB0'
    values_x = []
    values_y = []
    average_x= 0
    average_y = 0
    prevYaw = 0
    T1 = serial.Serial(port1, 115200, timeout=.1, write_timeout=0.1)

    prevX_comms = 0
    prevY_comms = 0
    def __init__(self):
        super().__init__('tagPublisher')
        self.publisher_ = self.create_publisher(Odometry , '/globalOdom', 10)
        self.timer = self.create_timer(.01, self.publish_PoseStamped)

    def publish_PoseStamped(self):
        self.comms()
        odom = Odometry()
        odom.header.frame_id = "globalOdom"
        odom.header.stamp = self.get_clock().now().to_msg()
        odom.pose.pose.position.x = float(self.average_x)
        odom.pose.pose.position.y = float(self.average_y)
        odom.pose.pose.position.z = 0.0
        self.mag = math.sqrt(self.dx*self.dx+self.dy*self.dy)
        self.yaw = math.atan2(self.dy, self.dx) 
        if self.dx < .07 and self.dy < .07:
            self.yaw = self.prevYaw
        quat = quaternion_from_euler(0, 0, self.yaw)
        odom.pose.pose.orientation = Quaternion(x=quat[0], y=quat[1], z=quat[2], w=quat[3])
        odom.child_frame_id = "base_link"
        odom.twist.twist.linear.x = 0.0#(self.dx)/(self.get_clock().now()-self.prevTime)
        odom.twist.twist.linear.y = 0.0#(self.dy)/(self.get_clock().now()-self.prevTime)
        odom.twist.twist.linear.z = 0.0
        self.prevTime = self.get_clock().now()
        #print("Publishing point: ", self.x_1, self.y_1)
        self.publisher_.publish(odom)
        self.prevYaw = self.yaw
    def comms(self):
        try:
            #print("\n---------------T1---------------")
            T1_in = self.T1.readline().decode().strip()
            #print(T1_in)
            if T1_in.startswith("P="):  # Check if the received data starts with "P="
                data = T1_in.split('=')[1].split(',')
                if len(data) == 3:  # Check if there are two values separated by ","
                    self.x_1 = self.x_2
                    self.y_1 = self.x_2

                    self.x_2 = float(data[0])
                    self.y_2 = float(data[1])
                    
                    if abs(self.x_2-self.x_1) > 8.5 or abs(self.y_2-self.y_1) > 8.5:
                        self.x_2 = self.x_1
                        self.y_2 = self.x_1

                self.values_x.append(self.x_2)
                self.values_y.append(self.y_2)

                # Check if there are 5 values in the list
                if len(self.values_x) == 5:
                    # Calculate the average of the 5 values
                    self.prevX = self.average_x
                    self.prevY = self.average_y

                    self.average_x = sum(self.values_x) / 5
                    self.average_y = sum(self.values_y) / 5
                    self.dx = self.average_x - self.prevX
                    self.dy = self.average_y - self.prevY


                    # Clear the list for the next set of values
                    self.values_x.clear()
                    self.values_y.clear()

        except Exception as e:
            print("exception:", e)


def main(args=None):
    rclpy.init(args=args)
    node = tagPublisher()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
