#include "../include/osk_chatroom.h"

#include <WinSock2.h>

int InitWinSock(int* initResult) {
    WSADATA wsaData;

    *initResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (*initResult != 0) {
        return OSK_FAILURE;
    }

    return OSK_SUCCESS;
}

int InitSocket(SOCKET* sock) {
    *sock = socket(AF_INET, SOCK_STREAM, 0);
    if (*sock == INVALID_SOCKET) {
        WSACleanup();
        return OSK_FAILURE;
    }

    return OSK_SUCCESS;
}

int CreateServer(Server* server, osk_error* error) {

    server->currentConnections = 0;
    server->max_sNumber = 0;

    for(int ii = 0; ii < MAX_CONNECTIONS; ii++) {
        server->clients[ii].sock = INVALID_SOCKET;
    }

    int initResult;
    if (InitWinSock(&initResult)) {
        error->message = "WSASartup failed with error: " + initResult;
        error->code = initResult;
        return OSK_FAILURE;
    }

    SOCKET s;
    if (InitSocket(&s)) {
        error->code = WSAGetLastError();
        error->message = "Socket creation failed with error: " + error->code;
        return OSK_FAILURE;
    }

    server->sock = s;

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    server->serverAddr = serverAddr;

    initResult = bind(s, (struct sockaddr*)&serverAddr, sizeof(serverAddr));
    if (initResult == SOCKET_ERROR) {
        error->code = WSAGetLastError();
        error->message = "Bind failed with error: " + error->code;
        closesocket(s);
        WSACleanup();
        return OSK_FAILURE;
    }

    u_long mode = 1;
    if (ioctlsocket(server->sock, FIONBIO, &mode) != 0) {
        error->message = "ioctlsocket failed";
        closesocket(server->sock);
        WSACleanup();
        return OSK_FAILURE;
    }

    initResult = listen(s, SOMAXCONN);
    if (initResult == SOCKET_ERROR) {
        error->code = WSAGetLastError();
        error->message = "Listen failed with error: " + error->code;
        closesocket(s);
        WSACleanup();
        return OSK_FAILURE;
    }
    return OSK_SUCCESS;
}

void CleanUp(Server* server) {
    closesocket(server->sock);
    WSACleanup();
}

int ConnectToServer(Client* client, char* ip, int port, osk_error* error) {
    int initResult;
    if (InitWinSock(&initResult)) {
        error->message = "WSASartup failed with error: ";
        error->code = initResult;
        return OSK_FAILURE;
    }

    SOCKET s;
    if (InitSocket(&s)) {
        error->code = WSAGetLastError();
        error->message = "Socket creation failed with error: ";
        return OSK_FAILURE;
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip);

    initResult = connect(s, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
    if (initResult == SOCKET_ERROR) {
        error->code = WSAGetLastError();
        error->message = "Connect failed with error: ";
        closesocket(s);
        WSACleanup();
        return OSK_FAILURE;
    }

    u_long mode = 1;
    if (ioctlsocket(s, FIONBIO, &mode) != 0) {
        error->code = WSAGetLastError();
        error->message = "ioctlsocket failed with error: ";
        closesocket(client->sock);
        WSACleanup();
        return OSK_FAILURE;
    }

    client->sock = s;

    // Sending username
    send(client->sock, client->username, USERNAME_SIZE, 0);

    return OSK_SUCCESS;
}