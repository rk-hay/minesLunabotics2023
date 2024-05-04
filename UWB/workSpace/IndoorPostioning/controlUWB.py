import serial
import time

UWB1 = '/dev/ttyUSB0' #update Udev rules


T1 = serial.Serial(UWB1, baudrate=115200, timeout=.001)

T1_REC = False
x_1 = 0
y_1 = 0
x_2=0
y_2=0
values_x = []
values_y = [] 
def uwb_Control():
    global x_1, x_2, y_1, y_2, values_x, values_y
    #print(frame_index)
    try:
        #print("\n---------------T1---------------")
        T1_in = T1.readline().decode().strip()
        #print(T1_in)
        if T1_in.startswith("P="):  # Check if the received data starts with "P="
            data = T1_in.split('=')[1].split(',')
            if len(data) == 3:  # Check if there are two values separated by ","
                x_1 = x_2
                y_1 = x_2

                x_2 = float(data[0])
                y_2 = float(data[1])

                if abs(x_2-x_1) > 8.5 or abs(y_2-y_1) > 8.5:
                    x_2 = x_1
                    y_2 = x_1

            values_x.append(x_2)
            values_y.append(y_2)

            # Check if there are 5 values in the list
            if len(values_x) == 5:
                # Calculate the average of the 5 values
                average_x = sum(values_x) / 5
                average_y = sum(values_y) / 5

                # Do something with the averages (e.g., print them)
                print("Average X:", average_x)
                print("Average Y:", average_y)

                # Clear the list for the next set of values
                values_x.clear()
                values_y.clear()

    except Exception as e:
        print("exception:", e)

while(True):
    uwb_Control()
