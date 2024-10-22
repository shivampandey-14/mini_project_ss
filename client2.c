#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

int main() 
{
    
    char* server_address = "127.0.0.1";  // We're writing loopback address or local host addr bcz server is on same machine
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        perror("Socket error");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, server_address, &serverAddr.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == -1) {
        perror("Connection failed");
        close(clientSocket);
        exit(1);
    }

    printf("Connected to the server\n");
    while (1) {
        // Receive data from the server
        char buffer[1024];
        memset(buffer, 0, sizeof(buffer));
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead <= 0) {
            // Connection closed by the server or an error occurred
            break;
        }

        // Process and display received data
        printf("Received: %s", buffer);

        char response[1024];
        char send_response[8];
        printf("Do you want to send a response? Press 'Y' or 'N': ");
        fgets(send_response, sizeof(send_response), stdin);
        send_response[strcspn(send_response, "\n")] = 0; // Remove newline character

        if (strcmp(send_response, "Y") == 0) {
            printf("Enter a message to send to the server: ");
            fgets(response, sizeof(response), stdin);
            response[strcspn(response, "\n")] = 0; // Remove newline character
            send(clientSocket, response, strlen(response), 0);
        } else if (strcmp(send_response, "N") == 0) {
            // Exit the loop if 'N' is pressed
            continue;
        } else {
            printf("Invalid input by user.\n");
        }
    }

    // Close the socket
    close(clientSocket);
    return 0;

}