import socket

#https://yifan-online.com/en/km/article/detail/2421
#https://gektor650.medium.com/esp-idf-esp32-udp-broadcasts-messages-through-the-wi-fi-4a7f3d75d8ea

UDP_IP = "255.255.255.255" # Broadcast address, usually 255.255.255.255 or fixed LAN address
UDP_PORT = 8888 # broadcast port

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
sock.bind((UDP_IP, UDP_PORT))

MESSAGE = b"Hello, World!"
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))