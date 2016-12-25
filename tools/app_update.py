# -*- coding: utf-8 -*-
import sys
import socket
import struct
import time

UPDATE_PORT = 54322
MAX_CHUNK_SIZE = 1024

updateSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#updateSocket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
updateSocket.bind(('', UPDATE_PORT))


def progress(count, total, suffix=''):
    bar_len = 60
    filled_len = int(round(bar_len * count / float(total)))

    percents = round(100.0 * count / float(total), 1)
    bar = '=' * filled_len + '-' * (bar_len - filled_len)

    sys.stdout.write('[%s] %s%s ...%s\r' % (bar, percents, '%', suffix))
    sys.stdout.flush()

   # Print iterations progress
def printProgress (iteration, total, prefix = '', suffix = '', decimals = 1, barLength = 100):
    """
    Call in a loop to create terminal progress bar
    @params:
        iteration   - Required  : current iteration (Int)
        total       - Required  : total iterations (Int)
        prefix      - Optional  : prefix string (Str)
        suffix      - Optional  : suffix string (Str)
        decimals    - Optional  : positive number of decimals in percent complete (Int)
        barLength   - Optional  : character length of bar (Int)
    """
    formatStr = "{0:." + str(decimals) + "f}"
    percent = formatStr.format(100 * (iteration / float(total)))
    filledLength = int(round(barLength * iteration / float(total)))
    bar = '█' * filledLength + '-' * (barLength - filledLength)
    sys.stdout.write('\r%s |%s| %s%s %s' % (prefix, bar, percent, '%', suffix)),
    if iteration == total:
        sys.stdout.write('\n')
    sys.stdout.flush()


def bytes(int32):
	ret = ""
	ret += chr(int32 & 0xff)
	ret += chr((int32>>8) & 0xff)
	ret += chr((int32>>16) & 0xff)
	ret += chr((int32>>24) & 0xff)
	return ret

def trySend(socket, address, data, timeout=1):
	gotResponse = False
	retries = 0
	#set socket timeout
	updateSocket.settimeout(timeout)
	while(gotResponse == False and retries < 3):
		try:
			socket.sendto(data, address)
			reply, fromAddr = updateSocket.recvfrom(100)
			if fromAddr == address:
				return reply
			else:
				retries+=1
				print "try send retry..."
		except:
			retries +=1
	return "ERROR"


if len(sys.argv) < 2:
	print "Please specify device ip address and binary path!"
else:
	deviceIp = sys.argv[1]
	binaryPath = sys.argv[2]

	#open image file and load into memory
	imageFile = ""
	with open(binaryPath, mode='rb') as file:
		imageFile = file.read()

	print "Firmware File " + sys.argv[2] + "opened. Size: " + str(len(imageFile)) + " bytes."
	numPacketsRequired = len(imageFile) / MAX_CHUNK_SIZE
	lastChunkSize = len(imageFile) % MAX_CHUNK_SIZE
	if(lastChunkSize != 0):
		numPacketsRequired += 1
	else:
		lastChunkSize = MAX_CHUNK_SIZE;

	imageMetadata = "UPDM" #update metadata
	#build 0 sequence number
	imageMetadata += bytes(0)
	#build image size in bytes
	imageMetadata += bytes(len(imageFile))
	#build num packets
	imageMetadata += bytes(numPacketsRequired)

	#updateSocket.sendto(imageMetadata, (deviceIp, UPDATE_PORT))
	trySend(updateSocket, (deviceIp, UPDATE_PORT), imageMetadata)

	#now we need to build the individual firmware update packets
	print ("Beginning update...\r\n")
	imageIndex = 0
	for x in range(0, numPacketsRequired-1):#numPacketsRequired-1):
		updateData = "UPDD" #update data
		updateData += bytes(x+1)
		updateData += bytes(MAX_CHUNK_SIZE)
		updateData += imageFile[imageIndex:(imageIndex+MAX_CHUNK_SIZE)]
		imageIndex += MAX_CHUNK_SIZE
		if(trySend(updateSocket, (deviceIp, UPDATE_PORT), updateData) == "ERROR"):
			exit(0);

		#progress(x, numPacketsRequired);
		printProgress(x, numPacketsRequired, prefix = 'Progress', suffix = 'Done', barLength = 50)

	updateData = "UPDD" #update data
	updateData += bytes(numPacketsRequired)
	updateData += bytes(lastChunkSize)
	updateData += imageFile[imageIndex:(imageIndex+lastChunkSize)]
	trySend(updateSocket, (deviceIp, UPDATE_PORT), updateData)
	#progress(numPacketsRequired, numPacketsRequired);
	printProgress(numPacketsRequired, numPacketsRequired, prefix = 'Progress', suffix = 'Done\r\n', barLength = 50)