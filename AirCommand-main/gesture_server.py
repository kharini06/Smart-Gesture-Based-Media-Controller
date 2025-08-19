import socket
import pyautogui
from pynput.keyboard import Controller

# Server configuration
host = "0.0.0.0"  # Listen on all available interfaces
port = 12345      # Port should match the ESP32 `serverPort`

# Initialize keyboard controller
keyboard = Controller()

# Function to perform actions based on gesture
def perform_action(gesture):
    if gesture == 'L':
        print("Left-Right gesture detected. Lowering volume...")
        for i in range(5):
            pyautogui.press('volumedown')
    elif gesture == 'U':
        print("Up-Down gesture detected. Increasing volume...")
        for i in range(3):    
            pyautogui.press('volumeup')
    elif gesture == 'T':
        print("Tap gesture detected. No action taken.")

# Set up the server
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((host, port))
server_socket.listen(1)

print("Server is listening for gestures...")

try:
    while True:
        client_socket, addr = server_socket.accept()
        print(f"Connection from {addr}")

        # Read gesture data from client
        data = client_socket.recv(1024).decode('utf-8').strip()
        print(f"Received gesture: {data}")

        # Process gesture
        if data:
            perform_action(data)

        client_socket.close()

except KeyboardInterrupt:
    print("Server shutting down.")
finally:
    server_socket.close()