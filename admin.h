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

bool handle_admin(int client_socket)
{
    printf("Inside admin.\n");
    while(1)
    {
        // send(client_socket,"Enter username: ",strlen("Enter username: "),0);
        char username_buffer[128];
        char password_buffer[128];
        int userid_buffer;
        char atoi_userid_buff[1024];
        int menu_options_recv;
        char atoi_buff[1024];  //to receive data from client
        int rec_client = recv(client_socket,&username_buffer,sizeof(username_buffer),0);
        username_buffer[rec_client] = '\0';
        printf("admin line35: %s\n",username_buffer);
        
        if(rec_client == -1)
        {
            perror("recv error in admin user_name\n");
            return false;
        }
        //send(client_socket,"Enter password: ",strlen("Enter password: "),0);
        rec_client = recv(client_socket,&password_buffer,sizeof(password_buffer),0);
        printf("admin line44 %s\n",password_buffer);
        if(rec_client == -1)
        {
            perror("recv error admin password\n");
            return false;
        }
        password_buffer[rec_client] = '\0';
        // send(client_socket,"Enter user_id: ",strlen("Enter user_id: "),0);
        rec_client = recv(client_socket,&userid_buffer,sizeof(userid_buffer),0);
        printf("admin line53 %d\n",userid_buffer);
        //userid_buffer = atoi(atoi_userid_buff);
        if(rec_client == -1)
        {
            perror("recv error admin user_id\n");
            return false;
        }
        if(authenticate_admin(client_socket,userid_buffer,password_buffer))
        {
            printf("admin authenticated\n");
            send(client_socket,"admin authenticated",strlen("admin authenticated"),0);
            //send(client_socket,"Authentication Successful",strlen("Authentication Successful"),0);
            while(1)
            {
                int menu_options_recv;
               // char menu_options[] = "Select action to perform(Select number corresponding to desired action):\n1)Add New Bank Employee\n2)Modify Customer\n3)Modify Employee\n4)Manage User Roles\n5)Change password\n6)logout\n7)exit\n";
                // send(client_socket,menu_options,strlen(menu_options),0);
                printf("admin 71\n");
                int client_rcv = recv(client_socket,&menu_options_recv,sizeof(menu_options_recv),0);
                printf("admin 73:%d\n",menu_options_recv);
                if(client_rcv == -1)
                {
                    perror("recv error admin menu_options_recv\n");
                    return false;
                }
                //atoi_buff[client_rcv] = '\0';
                int rec_cust_id;
                int customer_id;
                //send(client_socket,"reached\n",strlen("reached\n"),0);
                 
                //test:
                //send(client_socket,&menu_options_recv,sizeof(menu_options_recv),0);
                switch(menu_options_recv)
                {
                    case 1:
                    add_employee(client_socket);
                    break;
                    case 2:
                    edit_customer(client_socket);
                    break;
                    case 3:
                    edit_employee(client_socket);
                    break;
                    case 4:
                    // send(client_socket,"Enter employee id that you want to make manager.",strlen("Enter employee id that you want to make manager."),0);
                    int employee_id;
                    char emp_id_buff[1024];
                    int rcv_emp_id = recv(client_socket,&emp_id_buff,sizeof(emp_id_buff),0);
                    employee_id = atoi(emp_id_buff);
                    if(rcv_emp_id == -1)
                    {
                        perror("recv error admin_lin98");
                        return false;
                    }
                    manage_user_role(client_socket,employee_id);
                    break;
                    case 5:
                    edit_credentials_admin(client_socket,userid_buffer);
                    break;
                    case 6:
                    //logout will not terminate connection.
                    admin_logout(client_socket,userid_buffer);
                    break;
                    case 7:
                    //exit: it will terminate client server connection.
                    send(client_socket,"Exiting..",strlen("Exiting.."),0);
                    close(client_socket);
                    exit(0);
                    break;
                    default:
                    //send(client_socket,"Invalid input...\n",strlen("Invalid input"),0);
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