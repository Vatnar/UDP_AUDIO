from socket import *
serverName = "localhost"
serverPort = 12000
# SOCK_STREAM sets to use TCP, AF_INET use IPV4
# Dont specify port of client, let system do taht
clientSocket = socket(AF_INET, SOCK_STREAM)
# Initiate three way handshake
clientSocket.connect((serverName, serverPort))
sentence = input('Input lowercase sentence:')
clientSocket.send(sentence.encode())
modifiedSentence = clientSocket.recv(1024)
print('From server: ', modifiedSentence.decode())
clientSocket.close()