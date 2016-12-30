import socket
import struct

MCAST_GRP = '224.1.1.1'
MCAST_PORT = 50008

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

class Beacon:
	def __init__(self, data):
		dataIndex = 0
		self.light_state = ord(data[dataIndex])
		dataIndex += 1
		self.motion_trigger = ord(data[dataIndex])
		dataIndex += 1
		self.motion_trigger_time_seconds = int32FromBytes(data[dataIndex:dataIndex+4])
		dataIndex += 4
		self.firmware_version_major = ord(data[dataIndex+2])
		self.firmware_version_minor = ord(data[dataIndex+1])
		self.firmware_version_patch = ord(data[dataIndex+0])
		dataIndex += 4

	def __str__(self):
		ret = "Beacon[\r\n\tlight state: " + str(self.light_state) + "\r\n\tmotion trigger: "
		ret += str(self.motion_trigger) + "\r\n\tmotion trigger time: " + str(self.motion_trigger_time_seconds) + "\r\n\tfirmware version: "
		ret += str(self.firmware_version_major) + "." + str(self.firmware_version_minor) + "." + str(self.firmware_version_patch)
		ret += "\r\n];\r\n"
		return ret

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM, socket.IPPROTO_UDP)
sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
sock.bind(('', MCAST_PORT))  # use MCAST_GRP instead of '' to listen only
                             # to MCAST_GRP, not all groups on MCAST_PORT
mreq = struct.pack("4sl", socket.inet_aton(MCAST_GRP), socket.INADDR_ANY)

sock.setsockopt(socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, mreq)


while True:
  data, address = sock.recvfrom(128)
  if(data[0:4] == "NMAN" and int32FromBytes(data[8:12]) == 2): #if this has marker and is of type beacon
  	beacon = Beacon(data[16:])
  	print "beacon from: ", address, str(beacon)
