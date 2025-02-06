from socket import *
serverPort = 12000
serverSocket = socket(AF_INET, SOCK_STREAM)
serverSocket.bind(('', serverPort))
# Listen for connection, max 1 queue
serverSocket.listen(1)
print('Waiting for a connection...')

while True:
    # Create a new socket for each request
    connectionSocket, addr = serverSocket.accept()
    sentence = connectionSocket.recv(1024).decode()
    capitalizeSentence = sentence.upper()
    connectionSocket.send(capitalizeSentence.encode())
    print('Received sentence:', sentence, 'from address:', connectionSocket.getsockname(), 'port:', connectionSocket.getpeername())
    connectionSocket.close()