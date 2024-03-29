# -*- coding: utf-8 -*-


import socket
import datetime

 

localIP     = "10.148.132.251"      #Own IP

localPort   = 51885

bufferSize  = 4096

 

msgFromServer       = "Hello UDP Client"

bytesToSend         = str.encode(msgFromServer)

 

# Create a datagram socket

UDPServerSocket = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)

 

# Bind to address and ip

UDPServerSocket.bind((localIP, localPort))

 

print("UDP server up and listening")

 

# Listen for incoming datagrams

while(True):

    bytesAddressPair = UDPServerSocket.recvfrom(bufferSize)

    message = bytesAddressPair[0]

    address = bytesAddressPair[1]

    clientMsg = "Message from Client:{}".format(message)
    clientIP  = "Client IP Address:{}".format(address)
    
    print(clientMsg)
    print(clientIP)
    
    currentDT = datetime.datetime.now()
        
    DateTime = currentDT.strftime('%Y-%m-%d %H:%M:%S')
    
    row = DateTime + ',' + format(message) + '\n';
    
    csv_out = open('BoogieData.csv','a')
    csv_out.write(row)
    csv_out.close()

   

    # Sending a reply to client

    #####################################UDPServerSocket.sendto(bytesToSend, address)