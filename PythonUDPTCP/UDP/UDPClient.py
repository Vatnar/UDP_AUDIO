from socket import *
# Define server ip and port (locahost for 127.0.0.1)
serverName = 'localhost'
serverPort = 12000

# Initialize socket to send and receive from SOCK_DGRAM sets UDP
clientSocket = socket(AF_INET, SOCK_DGRAM)

# get message to send into socket
message = input('Input lowercase sentence:')

# Encode message to bytes and send to server
clientSocket.sendto(message.encode(),(serverName, serverPort))

# Read address and message
modifiedMessage, serverAddress = clientSocket.recvfrom(2048)

# print
print(modifiedMessage.decode())
clientSocket.close()