# Muhammad Ghazi
# 11/07/2023
# CSCE 3530.002 (12199)

import socket
import random
import sys

print("Muhammad Ghazi EUID-11469986")


# Set up the hostname and port
hostname = socket.gethostname()
port = 12345  # Port to use 

# Create a UDP socket
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_socket.bind((hostname, port))

print("Server is waiting for a client...")

try:
    while True:
        try:
            # Wait for a message from the client
            message, address = server_socket.recvfrom(1024)
            message = message.decode()

            if message == 'PING':
                print(f'Received PING from {address}')
                if random.random() < 0.7:  # 70% chance to respond
                    server_socket.sendto('PONG'.encode(), address)
                    print(f'Sent PONG to {address}')
                else:
                    # Simulate packet drop
                    print(f'Packet from {address} dropped')
        except socket.timeout:
            print("Server has been idle for 20 seconds, shutting down.")
            break
except KeyboardInterrupt:
    print("\nServer is shutting down.")
    server_socket.close()
    sys.exit(0)