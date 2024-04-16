import serial
import time

UWB1 = 'COM5' #update Udev rules
UWB2 = 'COM14'

T1 = serial.Serial(UWB1, baudrate=115200, timeout=.0001)
T2 = serial.Serial(UWB2, baudrate=115200, timeout=.0001)

prevFrameindex = 20
T1_REC = False
T2_REC = False
def uwb_Control():
    global prevFrameindex, T1_REC, T2_REC
    num_devices = 2
    frame_duration = .2#.2  # 200 ms
    frame_size = frame_duration / num_devices
    current_time = time.time()
    frame_index = int(current_time / frame_size) % num_devices
    #print(frame_index)
    try:
        #print("\n---------------T1---------------")
        T1_in = T1.readline().decode().strip()
        #print(T1_in)
        if T1_in.startswith("P="):  # Check if the received data starts with "P="
            data = T1_in.split('=')[1].split(',')
            if len(data) == 2:  # Check if there are two values separated by ","
                x = float(data[0])
                y = float(data[1])
                print("Received position 1:", x, y)
        if T1_in == "c":
            #print("-----------T1 REC-----------")
            T1_REC = True  # If T1 has read 4 distances
        #print("\n---------------T2---------------")
        T2_in = T2.readline().decode().strip()
        #print(T2_in) #IF T2 has read 4 distances
        if T2_in.startswith("P="):  # Check if the received data starts with "P="
            data2 = T2_in.split('=')[1].split(',')
            if len(data2) == 2:  # Check if there are two values separated by ","
                x_2 = float(data2[0])
                y_2 = float(data2[1])
                print("Received position 2:", x_2, y_2)
        if T2_in == "c":  # If T2 has read 4 distances
            T2_REC = True
            #print("-----------T2 REC-----------")

        if T2_REC == True and T1_REC == True:
            #print("-----------FRAMING-----------")
            if frame_index == 1:
                T1.write(b'A')  # T1 become Anchor
                T2.write(b'T') #T2 become Tag
            elif frame_index == 0:
                T1.write(b'T')
                T2.write(b'A')

    except Exception as e:
        print("exception:", e)
    time.time()

while(True):
    uwb_Control()
