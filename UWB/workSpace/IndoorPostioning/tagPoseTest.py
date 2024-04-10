import socket
import time

hostname = socket.gethostname()
UDP_IP =  socket.gethostbyname(hostname)
print("***Local ip:" + '192.168.111.8' + "***")
UDP_PORT_1 = 5000

sock_1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock_1.bind(('192.168.111.8', UDP_PORT_1))

sock_1.listen(1) 
data_1, addr_1 = sock_1.accept()



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
            if len(parts_1) == 4:
                x_1 = float(parts_1[0])
                y_1 = float(parts_1[1])
                x_2 = float(parts_1[2])
                y_2 = float(parts_1[3])
                print("data 1: ")
                print(x_1)
                print(y_1)
                print(x_2)
                print(y_2)
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
    
while True:
    read_data_1()
