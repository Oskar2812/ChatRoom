#include "../include/osk_chatroom.h"

#include <WinSock2.h>
#include <stdio.h>
#include <string.h>

int main() {
    osk_error error;
    Server server;
    if (CreateServer(&server, &error)) {
        printf("%s\n", error.message);
        printf("Server creation failed. Exiting...\n");
        return 1;
    }

    printf("Server is listening on: %d\n", server.serverAddr.sin_port);

    while(1) {
        FD_ZERO(&server.readfds);

        int tempSock = -1;
        FD_SET(server.sock, &server.readfds);
        for (int ii = 0; ii < MAX_CONNECTIONS; ii++) {
            tempSock = server.clients[ii].sock;
            if (tempSock > 0) {
                FD_SET(tempSock, &server.readfds);
            }
            if (tempSock > server.max_sNumber) {
                server.max_sNumber = tempSock;
            }
        }

        int activity = select(server.max_sNumber + 1, &server.readfds, NULL, NULL, NULL);
        if (activity == SOCKET_ERROR) {
            printf("Select failed\n");
            break;
        }

        if (FD_ISSET(server.sock, &server.readfds)) {
            struct sockaddr_in tempAddr;
            int tempLen = sizeof(tempAddr);
            SOCKET newSock = accept(server.sock, (struct sockaddr*)&tempAddr, &tempLen);
            if (newSock == INVALID_SOCKET) {
                printf("Client connection failed\n");
                continue;
            }
            char tempUsername[USERNAME_SIZE]; 
            int valread = recv(newSock, tempUsername, USERNAME_SIZE, 0);
            if (valread == 0) {
                printf("New client did not send username");
            }
            else if (valread < 0) {
                int error = WSAGetLastError();
                printf("New client failed to send username with error: %d\n", error);
            } 

            printf("New connection from: %s\n", tempUsername);

            for (int ii = 0; ii < MAX_CONNECTIONS; ii++) {
                if (server.clients[ii].sock == INVALID_SOCKET) {
                    server.clients[ii].sock = newSock;
                    server.clients[ii].clientAddr = tempAddr;
                    strcpy(server.clients[ii].username, tempUsername);
                    break;
                }
            }
        }

        for(int ii = 0; ii < MAX_CONNECTIONS; ii++) {
            if (FD_ISSET(server.clients[ii].sock, &server.readfds)) {
                int valread = recv(server.clients[ii].sock, server.buffer, BUFFER_SIZE, 0);
                if (valread == 0) {
                    FD_CLR(server.clients[ii].sock, &server.readfds);
                    printf("%s has disconnected\n", server.clients[ii].username);
                    closesocket(server.clients[ii].sock);
                    server.clients[ii].sock = INVALID_SOCKET;
                    continue;
                }
                else if (valread > 0) {
                    server.buffer[valread] = '\0';
                    char message[BUFFER_SIZE];
                    sprintf(message, "[%s] ", server.clients[ii].username);
                    strcat(message, server.buffer);
                    FD_CLR(server.clients[ii].sock, &server.readfds);
                    for (int jj = 0; jj < MAX_CONNECTIONS; jj++) {
                        if (ii == jj) {
                            continue;
                        }
                        if (server.clients[jj].sock != INVALID_SOCKET) {
                            send(server.clients[jj].sock, message, BUFFER_SIZE, 0);
                        }
                    }
                }
                else {
                    int error = WSAGetLastError();
                    if (error == 10035) {
                        continue;
                    }
                    if (error == 10054) {
                        printf("%s has disconnected\n", server.clients[ii].username);
                        FD_CLR(server.clients[ii].sock, &server.readfds);
                        closesocket(server.clients[ii].sock);
                        server.clients[ii].sock = INVALID_SOCKET;
                        continue;
                    }
                    else {
                        printf("Receive failed with error: %d\n", error);
                    }
                }
            }
        }

    }

    

    closesocket(server.sock);
    WSACleanup();
    return 0;
}