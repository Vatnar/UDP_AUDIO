#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "27015"
#define LOG_FILE "serverlog.txt"

// Logging function to write messages to a file
void log_message(const char *message) {
    // Open file in append mode
    FILE *logFile = fopen(LOG_FILE, "a");
    if (logFile != nullptr) { const time_t now = time(nullptr);
        fprintf(logFile, "[%s] %s\n", ctime(&now), message);
        fclose(logFile);
    }
}

int __cdecl main(void) {
    // Init WSADATA struct,
    WSADATA wsaData;

    int iResult;

    // Create Listen socket to listen for initial connection
    // Create a ClientSocket where we store information about the client.
    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    // Holds host address information
    // result is used for resolving hostnames to IP Addresses
    // hints is for specifying criteria.
    addrinfo *result = nullptr, hints{};

    int  iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int  recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock, MAKEWORD(2,2) Specifies the version of the API.
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        log_message("WSAStartup failed.");
        return 1;
    } // Failed.

    ZeroMemory(&hints, sizeof(hints)); // Initialize to 0.
    hints.ai_family = AF_INET; // Address Family _ INET: IP and port numbers https://www.ibm.com/docs/en/i/7.4?topic=family-af-inet-address

    // Sequenced, two-way byte streams. Reliable, in order. https://www.ibm.com/docs/ko/aix/7.1?topic=protocols-socket-types
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP; // https://learn.microsoft.com/en-us/windows/win32/winsock/ipproto-tcp-socket-options Set what protocol to use
    hints.ai_flags    = AI_PASSIVE; // Will be bound https://learn.microsoft.com/en-us/windows/win32/api/ws2def/ns-ws2def-addrinfoa

    // Resolve the server address and port
    iResult = getaddrinfo("0.0.0.0", DEFAULT_PORT, &hints, &result); // https://learn.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-getaddrinfo
    if (iResult != 0) {
        log_message("getaddrinfo failed.");
        WSACleanup();
        return 1;
    } // Failed to resolve address or port

    // Create a SOCKET for the server to listen for client connections.
    ListenSocket = socket( // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-socket
        result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        log_message("socket failed.");
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    } // Socket not valid

    // Set up the TCP listening socket, Associates local address with a socket
    iResult = bind(ListenSocket, result->ai_addr, (int) result->ai_addrlen); // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-bind
    if (iResult == SOCKET_ERROR) {
        log_message("bind failed.");
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result); // Frees address information that getaddrinfo() allocates

    iResult = listen(ListenSocket, SOMAXCONN); // SOMAXCONN maxed queued connections
    if (iResult == SOCKET_ERROR) {
        log_message("listen failed.");
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    } // If failed to listen

    // Permit an incoming connection on a socket
    ClientSocket = accept(ListenSocket, nullptr, nullptr);
    if (ClientSocket == INVALID_SOCKET) {
        log_message("accept failed.");
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    // No longer need server socket since it has "reached out"
    closesocket(ListenSocket);

    // Receive until the peer shuts down the connection
    do {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0); // https://learn.microsoft.com/en-us/windows/win32/api/winsock/nf-winsock-recv
        if (iResult > 0) { // returns number of bytes received
            char logBuffer[100];
            // snprintf writes to buffer in a printf type way
            snprintf(
                logBuffer, sizeof(logBuffer), "Bytes received: %d", iResult);
            log_message(logBuffer);

            // Echo the buffer back to the sender
            iSendResult = send(ClientSocket, recvbuf, iResult, 0); // https://learn.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-send
            if (iSendResult == SOCKET_ERROR) {
                log_message("send failed.");
                closesocket(ClientSocket);
                WSACleanup();
                return 1;
            } // Failed to send
            snprintf(logBuffer, sizeof(logBuffer), "Bytes sent: %d", iSendResult);
            log_message(logBuffer);

        } else if (iResult == 0)
            { log_message("Connection closing..."); } // Nothing received, then close
        else {
            log_message("recv failed.");
            closesocket(ClientSocket);
            WSACleanup();
            return 1;
        } // Something went wrong.
    }
    while (iResult > 0); // Keep going as long as we recieve data.

    // shutdown the connection since we're done
    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        log_message("shutdown failed.");
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    // cleanup
    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}
