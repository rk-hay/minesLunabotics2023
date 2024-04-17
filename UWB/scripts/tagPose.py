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
    port2 = '/dev/ttyUSB1' 
    currPort = port1
    targets = [b'2', b'3', b'4', b'5']
    target_index = 0
    attempt_no = 0
    T1 = serial.Serial(currPort, 115200, timeout=.1, write_timeout=0.1)
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
        odom.pose.pose.position.x = self.x_1
        odom.pose.pose.position.y = self.y_1
        odom.pose.pose.position.z = 0.0
        dx = self.x_2 - self.x_1
        dy = self.y_2 - self.y_1
        self.mag = math.sqrt(self.dx*self.dx+self.dy*self.dy)
        self.yaw = math.atan2(dy, dx) 
        quat = quaternion_from_euler(0, 0, self.yaw)
        odom.pose.pose.orientation = Quaternion(x=quat[0], y=quat[1], z=quat[2], w=quat[3])
        odom.child_frame_id = "base_link"
        odom.twist.twist.linear.x = 0.0#(self.x_1-self.prevX)/(self.get_clock().now()-self.prevTime)
        odom.twist.twist.linear.y = 0.0#(self.y_1-self.prevY)/(self.get_clock().now()-self.prevTime)
        odom.twist.twist.linear.z = 0.0
        self.prevX = self.x_1
        self.prevY = self.y_1
        self.prevTime = self.get_clock().now()
        #print("Publishing point: ", self.x_1, self.y_1)
        self.publisher_.publish(odom)

    def comms(self):
        try:
            T1_in = self.T1.readline().decode().strip()
            #print(T1_in)
            self.attempt_no += 1
            if self.attempt_no > 20:
                #print("watchdog " + str(self.currPort))
                self.tag()
            if T1_in.startswith("P="):  # Check if the received data starts with "P="
                data2 = T1_in.split('=')[1].split(',')
                if len(data2) == 2:  # Check if there are two values separated by ","
                    x = float(data2[0])
                    y = float(data2[1])
                    #print("Received position", x, y)
                    if x > 10 or x < -10 or y > 10 or y < -10:
                        x = self.prevX_comms
                        y = self.prevY_comms
                        self.prevX_comms = x
                        self.prevY_comms = y
                    if self.currPort == self.port1:
                        self.x_1 = x
                        self.y_1 = y
                    elif self.currPort == self.port2:
                        self.x_2 = x
                        self.y_2 = y
                    print("Received position", x, y)
            if T1_in.startswith("c="):
                distance = T1_in.split('=')
                #print(str(self.currPort) + " Index: " + str(self.target_index) + " D: " + str(distance[1]) + " Atmpt: " + str(self.attempt_no)) # after this has successfully ranged all 4 targets swap to UWB 2
                self.target_index += 1
                if self.target_index >= len(self.targets):    
                    self.target_index = 0
                    #change to new com port
                    self.currPort = self.port2 if self.currPort == self.port1 else self.port1
                    self.T1.close()
                    self.T1 = serial.Serial(self.currPort, 115200, timeout=.1, write_timeout=0.1)
                self.tag()

        except Exception as e:
            print("exception:", e)

    def tag(self):
        #print("watchdog " + str(self.currPort) + " " + str(self.target_index))
        self.T1.write(b't')
        self.T1.write(self.targets[self.target_index])
        self.attempt_no = 0


def main(args=None):
    rclpy.init(args=args)
    node = tagPublisher()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
