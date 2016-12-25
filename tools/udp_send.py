import socket
import sys

UDP_IP = "127.0.0.1"
UDP_PORT = 50008
MESSAGE = "light on"

if len(sys.argv) > 2:
	UDP_IP = sys.argv[1]
	MESSAGE = sys.argv[2];

print "UDP target IP:", UDP_IP
print "UDP target port:", UDP_PORT
print "message:", MESSAGE

sock = socket.socket(socket.AF_INET, # Internet
                     socket.SOCK_DGRAM) # UDP
sock.sendto(MESSAGE, (UDP_IP, UDP_PORT))