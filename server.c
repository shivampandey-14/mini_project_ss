#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include<stdbool.h>
#include<string.h>
#include "customer.h"
#include "employee.h"
#include "manager.h"
#include "admin.h"

#define PORT 8080
#define MAX_CLIENTS 5
#define BUFFER_SIZE 1024
void handle_client(int client_socket);
int main()
{
    //First create a socket
    int server_fd = socket(AF_INET,SOCK_STREAM,0);
    if(server_fd == -1)
    {
        perror("socket error");
        return 1;
    }
    struct sockaddr_in addr_server,addr_client;
    socklen_t addr_client_len = sizeof(addr_client);
    addr_server.sin_family = AF_INET;
    addr_server.sin_port = htons(PORT);
    addr_server.sin_addr.s_addr = INADDR_ANY;
    // Set SO_REUSEADDR option on the socket
    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1) {
        perror("setsockopt");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    //Now bind socket.
    if(bind(server_fd,(struct sockaddr*)&addr_server,sizeof(addr_server)) == -1)
    {
        perror("bind error");
        return 1;
    }
    if(listen(server_fd,MAX_CLIENTS) == -1)
    {
        perror("listen error");
        return 1;
    }
    while(1)
    {
        //this is loop for accepting incoming connection requests.
        int new_client = accept(server_fd,(struct sockaddr*)&addr_client,&addr_client_len);
        if(new_client == -1)
        {
            perror("accept error");
            close(server_fd);
            return 1;
        }
        pid_t pid = fork();
        if(pid == 0)
        {
            //we're in child process.
            //child process will handle each new client.
            //child does not need server fd.
            close(server_fd);
            handle_client(new_client);
        }
        else if(pid > 0)
        {
            //we're in parent process.
            //parent will keep on accepting new connections.
            close(new_client);
        }
        else
        {
            perror("fork error");
            return 1;
        }
    }
}
void handle_client(int client_socket)
{
    printf("INside Server Handle_client\n");
    while(1)
    {
        //char menu[] = "Select Role(Enter number corresponding desired role: )\n1)Customer\n2)Employee\n3)Manager\n4)Admin\n";
        int menu_choice;
        //char atoi_buff[1024];
        //send(client_socket,menu,strlen(menu),0);
        printf("server side before recv 92\n");
        int rec_client = recv(client_socket,&menu_choice,sizeof(menu_choice),0);
        printf("server side menu choice %d\n",menu_choice);
        //menu_choice = atoi(atoi_buff);
        switch(menu_choice)
        {
            case 1:
            handle_customer(client_socket);
            
                //send(client_socket,"Thank You.",strlen("Thank You."),0);
            
            break;
            case 2:
            handle_employee(client_socket);
            // {
            //     //send(client_socket,"Thank You.",strlen("Thank You."),0);
            // }
            break;
            case 3:
            handle_manager(client_socket);
            // {
                //send(client_socket,"Thank You.",strlen("Thank You."),0);
            // }
            break;
            case 4:
            handle_admin(client_socket);
            
                //send(client_socket,"Thank You.",strlen("Thank You."),0);
            
            break;
            default:
            // send(client_socket,"Invalid input\n",strlen("Invalid input"),0);
            break;
        }
    }
}