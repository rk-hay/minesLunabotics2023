
import serial
import time
import threading

class uwb_Control():
    port1 = '/dev/ttyUSB0'
    port2 = '/dev/ttyUSB1' 
    currPort = port1
    targets = [b'2', b'3', b'4', b'5']
    #targets = [b'5', b'5', b'5', b'5']
    target_index = 0
    attempt_no = 0
    T1 = serial.Serial(currPort, 115200, timeout=.1, write_timeout=0.1)
    x_1 = 0
    x_2 = 0
    y_1 = 0
    y_2 = 0
    prevX = 0
    prevY = 0
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
                        x = self.prevX
                        y = self.prevY
                        self.prevX = x
                        self.prevY = y
                    if self.currPort == 'COM5':
                        self.x_1 = x
                        self.y_1 = y
                    elif self.currPort == 'COM14':
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
        print("watchdog " + str(self.currPort) + " " + str(self.target_index))
        self.T1.write(b't')
        self.T1.write(self.targets[self.target_index])
        self.attempt_no = 0

# Flag to indicate if it's the first time ranging the targets
firstTime = True

UWB = uwb_Control()
while True:
    if firstTime:
        UWB.tag()
        firstTime = False
    else:
        UWB.comms()
