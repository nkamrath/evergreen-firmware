import socket
import struct

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 50009

#make a 32 bit in from bytes
def int32FromBytes(bytes):
	val = 0
	if(len(bytes) == 4):
		val |= (ord(bytes[0])) & 0xff
		val |= ((ord(bytes[1]) & 0xff) <<8)
		val |= ((ord(bytes[2]) & 0xff) <<16)
		val |= ((ord(bytes[3]) & 0xff) <<24)
	return val

#make a 32 bit in from bytes
def int16FromBytes(bytes):
	val = 0
	if(len(bytes) == 2):
		val |= (ord(bytes[0])) & 0xff
		val |= ((ord(bytes[1]) & 0xff) <<8)
	return val

def bytes(int32):
	ret = ""
	ret += chr(int32 & 0xff)
	ret += chr((int32>>8) & 0xff)
	ret += chr((int32>>16) & 0xff)
	ret += chr((int32>>24) & 0xff)
	return ret

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', MCAST_PORT))  # use MCAST_GRP instead of '' to listen only
                             # to MCAST_GRP, not all groups on MCAST_PORT
mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)

sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)

def announce_reply(address):
	#announce reply should jsut be a header with no data
	data = "NMAN" #marker
	data += bytes(0) #unused sequence number
	data += bytes(1) #packet type announce response
	data += bytes(0) #0 length payload
	sock.sendto(data, address)

while True:
  data, address = sock.recvfrom(128)
  if(data[0:4] == "NMAN"):
  	print "Got network manager announce from: ", address
  	announce_reply(address)
