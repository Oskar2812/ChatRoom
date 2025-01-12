#pragma once

#include <WinSock2.h>

#define OSK_SUCCESS 0
#define OSK_FAILURE 1

#define MAX_CONNECTIONS 10
#define BUFFER_SIZE 1024

#define PORT 8080

#define USERNAME_SIZE 32

typedef struct {
    struct sockaddr_in clientAddr;
    SOCKET sock;
    char buffer[BUFFER_SIZE];
    char username[USERNAME_SIZE];
} Client;

typedef struct {
    struct sockaddr_in serverAddr;
    SOCKET sock;
    Client clients[MAX_CONNECTIONS];
    int currentConnections;
    char buffer[BUFFER_SIZE];
    fd_set readfds;
    int max_sNumber;
} Server;

typedef struct {
    char* message;
    int code;
} osk_error;

int InitWinSock(int* result);
int InitSocket(SOCKET* sock);

int CreateServer(Server* server, osk_error* error);

int ConnectToServer(Client* client, char* ip, int port, osk_error* error);

void CleanUp(Server* server);

