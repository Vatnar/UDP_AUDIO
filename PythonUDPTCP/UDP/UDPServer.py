from socket import *

#set port for socket
serverPort = 12000
#init server socket
serverSocket = socket(AF_INET, SOCK_DGRAM)
#bind the socket to listen on that port
serverSocket.bind(('', serverPort))
print("The server is ready to receive")
# Listen for data directed to that port, do something then send back.
while True:
    message, clientAddress = serverSocket.recvfrom(2048)
    modifiedMessage = message.decode().upper()
    serverSocket.sendto(modifiedMessage.encode(), clientAddress)