import socket
import sys

UDP_IP = "127.0.0.1"
UDP_PORT = 50008
message = "light on"

def bytes(int32):
	ret = ""
	ret += chr(int32 & 0xff)
	ret += chr((int32>>8) & 0xff)
	ret += chr((int32>>16) & 0xff)
	ret += chr((int32>>24) & 0xff)
	return ret

def udpSendTo(ipAddress, port, commandString):
	#command header
	data = "NMAN" #marker
	data += bytes(0) #unused sequence number
	data += bytes(3) #packet type command
	data += bytes(len(commandString)) #0 length payload
	data += commandString

	sock = socket.socket(socket.AF_INET, # Internet
	                     socket.SOCK_DGRAM) # UDP
	sock.sendto(data, (ipAddress, port))

if __name__ == '__main__':

	if len(sys.argv) > 2:
		UDP_IP = sys.argv[1]
		message = sys.argv[2];

	print "UDP target IP:", UDP_IP
	print "UDP target port:", UDP_PORT
	print "message:", message

	udpSendTo(UDP_IP, UDP_PORT, message)