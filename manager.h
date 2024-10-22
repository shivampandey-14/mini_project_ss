#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<stdbool.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<errno.h>
#include <fcntl.h>
#include <sys/file.h>
#include <sys/socket.h>
#include "commonStruct.h"
#include "transaction.h"
#include "credentials.h"
#include "loan.h"
#include "feedback.h"
#include "transactionHistory.h"
#include "viewBalance.h"
#include "addEdit.h"

bool handle_manager(int client_socket)
{
    while(1)
    {
        //send(client_socket,"Enter username: ",strlen("Enter username: "),0);
        char username_buffer[128];
        char password_buffer[128];
        int userid_buffer;
        int menu_options_recv;
        int rec_client = recv(client_socket,&username_buffer,sizeof(username_buffer),0);
        username_buffer[rec_client] = '\0';
        if(rec_client == -1)
        {
            perror("recv error");
            return false;
        }
        //send(client_socket,"Enter password: ",strlen("Enter password: "),0);
        rec_client = recv(client_socket,&password_buffer,sizeof(password_buffer),0);
        if(rec_client == -1)
        {
            perror("recv error");
            return false;
        }
        password_buffer[rec_client] = '\0';
        //send(client_socket,"Enter user_id: ",strlen("Enter user_id: "),0);
        //char buff_usrid[1024];
        rec_client = recv(client_socket,&userid_buffer,sizeof(userid_buffer),0);
        //userid_buffer = atoi(buff_usrid);
        if(rec_client == -1)
        {
            perror("recv error");
            return false;
        }
        if(authenticate_manager(client_socket,userid_buffer,password_buffer))
        {
            send(client_socket,"Authentication Successful",strlen("Authentication Successful"),0);
            while(1)
            {
                //char menu_options[] = "Select action to perform(Select number corresponding to desired action):\n1)Deactivate account\n2)Show all applied loan applications\n3)Assign loan application to employee\n4)Review Feedback\n5)Change password\n6)logout\n7)exit\n";
                //send(client_socket,menu_options,strlen(menu_options),0);
                char buff_rec[1024];
                int client_rcv = recv(client_socket,buff_rec,sizeof(buff_rec),0);
                menu_options_recv = atoi(buff_rec);
                int rec_cust_id;
                int customer_id;
                switch(menu_options_recv)
                {
                    case 1:
                    //send(client_socket,"Enter customer id that you want to deactivate:",strlen("Enter customer id that you want to deactivate:"),0);
                    char buff_cuss[1024];
                    rec_cust_id = recv(client_socket,buff_cuss,sizeof(buff_cuss),0);
                    customer_id = atoi(buff_cuss);
                    if(rec_cust_id == -1)
                    {
                        perror("recv error");
                        return false;
                    }
                    deactivate_customer(customer_id);
                    break;
                    case 2:
                    view_applied_loan_applications();
                    break;
                    case 3:
                    assign_loan(client_socket);
                    break;
                    case 4:
                    //send(client_socket,"Enter customer id whose feedback you want to resolve.",strlen("Enter customer id whose feedback you want to resolve."),0);
                    // int usr_id;
                    // char buff_user[1024];
                    // int rec_usr_id = recv(client_socket,buff_user,sizeof(buff_user),0);
                    // usr_id = atoi(buff_user);
                    // if(rec_usr_id == -1)
                    // {
                    //     perror("rcv error");
                    //     return false;
                    // }
                    // resolve_feedback(usr_id);
                    break;
                    case 5:
                    edit_credentials_manager(client_socket,userid_buffer);
                    break;
                    case 6:
                    //logout will not terminate connection.
                    manager_logout(client_socket,userid_buffer);
                    break;
                    case 7:
                    //exit: it will terminate client server connection.
                    send(client_socket,"Exiting..",strlen("Exiting.."),0);
                    close(client_socket);
                    exit(0);
                    default:
                    send(client_socket,"Invalid input",strlen("Invalid input"),0);
                    break;
                }
                if(menu_options_recv == 6)
                {
                    //user has logged out so come out of while loop here.
                    return 1;
                }
            }
        }
        else
        {
            send(client_socket,"Authentication failed",strlen("Authentication failed"),0);
            return false;
        }
    }
}