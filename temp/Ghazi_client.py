# Muhammad Ghazi
# 11/07/2023
# CSCE 3530.002 (12199)

import socket
import sys

print("Muhammad Ghazi EUID-11469986")

# Set up the hostname and port
hostname = socket.gethostname()
port = 12345  # Port to use 

# Create a UDP socket
client_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
client_socket.settimeout(1)  # Set a timeout of 1 second

# Send PING message in a loop
for i in range(10):
    message = 'PING'.encode()
    addr = (hostname, port)

    try:
        # Send PING
        print(f'Sending PING {i + 1}')
        client_socket.sendto(message, addr)

        # Wait for PONG
        response, server = client_socket.recvfrom(1024)
        print('Received from server:', response.decode())

    except socket.timeout:
        print(f'PING {i + 1} timed out')

# Close the socket
client_socket.close()
