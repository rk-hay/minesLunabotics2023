import rclpy
from rclpy.node import Node
import serial
from time import sleep
import struct
from std_msgs.msg import Float64MultiArray
from sensor_msgs.msg import Joy
ser = serial.Serial('/dev/ttyACM0', baudrate=115200)
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
            Float64MultiArray,
            '/forward_velocity_controller/commands',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        ser.write(struct.pack('c', b'L'))
        ser.write(struct.pack('%sf' % len(msg.pub_vel), *msg.pub_vel))
        sleep(0.1)
            
class PosComm(Node):
    def __init__(self):
        super().__init__('PosComm')
        self.subscription = self.create_subscription(
            Float64MultiArray,
            '/forward_position_controller/commands',
            self.listener_callback,
            10)
        self.subscription  # prevent unused variable warning

    def listener_callback(self, msg):
        ser.write(struct.pack('c', b'P'))
        ser.write(struct.pack('%sf' % len(msg.pub_pos), *msg.pub_pos))
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
        global prevbutton
        global velSub

        if msg.buttons[3] != prevButton :
            ser.write(struct.pack('c', b'B'))
            ser.write(struct.pack('?', bool(msg.buttons[3]))) #MIGHT THROW ERROR
            prevButton = msg.buttons[3] # Y toggle autonomy
            sleep(0.1)

def main(args=None):
    rclpy.init(args=args)

    velocityComm = VelocityComm()
    posComm = PosComm()
    buttonPressComm = ButtonPressComm()

    rclpy.spin(buttonPressComm)
    rclpy.spin(velocityComm)
    rclpy.spin(posComm)


    ser.close()
    velocityComm.destroy_node()
    buttonPressComm.destroy_node()
    posComm.destroy_node()

    rclpy.shutdown()


if __name__ == '__main__':
    main()