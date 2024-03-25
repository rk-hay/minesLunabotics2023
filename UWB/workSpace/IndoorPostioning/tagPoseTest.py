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


def read_data():
    try:
        data.settimeout(5)  # Set a timeout of 5 seconds
        received_data = data.recv(1024).decode('utf-8')
        data.settimeout(None)  # Reset the timeout

        try:
            print(received_data)
            # Split the received data using comma as delimiter
            parts = received_data.split(',')
            print(parts)
            # Ensure all parts are present before assigning values
            if len(parts) == 3:
                x = float(parts[0])
                y = float(parts[1])
                e = float(parts[2])
                print(x)
                print(y)
                print(e)
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
while True:
    read_data()