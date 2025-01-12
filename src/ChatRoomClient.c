#include "../include/osk_chatroom.h"

#include <WinSock2.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <conio.h>

#define IP_ADDRESS "127.0.0.1"

int main() {
    Client client;
    osk_error error;

    char usernameBuffer[USERNAME_SIZE];
    printf("Please enter your username: ");
    fgets(usernameBuffer, sizeof(usernameBuffer), stdin);
    usernameBuffer[strcspn(usernameBuffer, "\n")] = 0;
    strcpy(client.username, usernameBuffer);

    if (ConnectToServer(&client, IP_ADDRESS, PORT, &error) != OSK_SUCCESS) {
        printf("%s%d\n", error.message, error.code);
        return 1;
    }

    printf("Welcome to OskChat!! You can now begin chatting\n");
    
    while(1) {
        if(kbhit()) {
            if(fgets(client.buffer, BUFFER_SIZE, stdin) != NULL){
                client.buffer[strcspn(client.buffer, "\n")] = 0;

                send(client.sock, client.buffer, strlen(client.buffer), 0);
            }
        }

        int valread = recv(client.sock, client.buffer, BUFFER_SIZE, 0);
        if (valread == 0) {
            printf("Server has disconnected you..\n");
            break;
        }
        else if (valread > 0) {
            printf("%s\n", client.buffer);
        }
    }

    closesocket(client.sock);
    WSACleanup();
    return 1;
}