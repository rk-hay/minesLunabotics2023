import socket
import time

hostname = socket.gethostname()
UDP_IP =  socket.gethostbyname(hostname)
print("***Local ip:" + '192.168.107.8' + "***")
UDP_PORT_1 = 5000
UDP_PORT_2 = 5001
sock_1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock_1.bind(('192.168.107.8', UDP_PORT_1))

sock_2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock_2.bind(('192.168.107.8', UDP_PORT_2))

sock_1.listen(1) 
data_1, addr_1 = sock_1.accept()

sock_2.listen(1) 
data_2, addr_2 = sock_2.accept()

def read_data_1():
    try:
        data_1.settimeout(5)  # Set a timeout of 5 seconds
        received_data_1 = data_1.recv(1024).decode('utf-8')
        data_1.settimeout(None)  # Reset the timeout

        try:
            print(received_data_1)
            # Split the received data using comma as delimiter
            parts_1 = received_data_1.split(',')
            print(parts_1)
            # Ensure all parts_1 are present before assigning values
            if len(parts_1) == 3:
                x_1 = float(parts_1[0])
                y_1 = float(parts_1[1])
                e_1 = float(parts_1[2])
                print("data 1: ")
                print(x_1)
                print(y_1)
                print(e_1)
            else:
                print("Incomplete data received:", received_data_1)
        except:
            print("Error:")
            print("Failed to parse received data:", received_data_1)
        print("")
        return received_data_1
    except socket.timeout:
        print("Socket timeout. No data received.")
        data_1.settimeout(None)
        return 0
    
def read_data_2():
    try:
        data_2.settimeout(5)  # Set a timeout of 5 seconds
        received_data_2 = data_2.recv(1024).decode('utf-8')
        data_2.settimeout(None)  # Reset the timeout

        try:
            print(received_data_2)
            # Split the received data using comma as delimiter
            parts_2 = received_data_2.split(',')
            print(parts_2)
            # Ensure all parts_1 are present before assigning values
            if len(parts_2) == 3:
                x_2 = float(parts_2[0])
                y_2 = float(parts_2[1])
                e_2 = float(parts_2[2])
                print("data 2: ")
                print(x_2)
                print(y_2)
                print(e_2)
            else:
                print("Incomplete data received:", received_data_2)
        except:
            print("Error:")
            print("Failed to parse received data:", received_data_2)
        print("")
        return received_data_2
    except socket.timeout:
        print("Socket timeout. No data received.")
        data_2.settimeout(None)
        return 0    
while True:
    read_data_1()
    read_data_2()