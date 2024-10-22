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
        int menu_choice;
        scanf("%d",&menu_choice);
        send(clientSocket,&menu_choice,sizeof(menu_choice),0);
        switch (menu_choice)
        {
        case 1:
            char username[1024];
            char password[1024];
            int userid;
            printf("Enter username: \n");
            scanf("%s",&username);
            send(clientSocket,username,strlen(username),0);
            printf("Enter password: \n");
            scanf("%s",&password);
            printf("Enter user_id: \n");
            scanf("%d",&userid);
            char auth_buffer[1024];
            int auth_rcv = recv(clientSocket,&auth_buffer,sizeof(auth_buffer),0);
            auth_buffer[auth_rcv] = '\0';
            if(strcmp(auth_buffer,"Authentication Successful") == 0)
            {
                while(1)
                {
                    char menu_options[1024];
                    int rec_menu = recv(clientSocket,&menu_options,strlen(menu_options),0);
                    if(rec_menu <= 0)
                    {
                        perror("receive error");
                        close(clientSocket);
                        return 0;
                    }
                    printf("%s",menu_options);
                    int user_choice;
                    scanf("%d",&user_choice);
                    send(clientSocket,&user_choice,sizeof(user_choice),0);
                    switch(user_choice)
                    {
                        case 2:
                        printf("Enter amount you want to deposit: ");
                        int deposit;
                        scanf("%d",&deposit);
                        send(clientSocket,&deposit,sizeof(deposit),0);
                        printf("Enter amount to withdraw: \n");
                        
                    }
                }
            }
        case 2:

        case 3:

        case 4:

        default:
            break;
        }
        
    }

    // Close the socket
    close(clientSocket);
    return 0;

}