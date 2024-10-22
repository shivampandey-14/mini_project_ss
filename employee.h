#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
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

bool handle_employee(int client_socket)
{
    while (1)
    {
        printf("Inside employee\n");
        // first authenticate then add other fucntionalities.
        // send(client_socket,"Enter username: ",strlen("Enter username: "),0);
        char username_buffer[128];
        char password_buffer[128];
        int userid_buffer;
        int menu_options_recv;
        int rec_client = recv(client_socket, &username_buffer, sizeof(username_buffer), 0);
        username_buffer[rec_client] = '\0';
        printf("emp 35 %s\n",username_buffer);
        if (rec_client == -1)
        {
            perror("recv error");
            return false;
        }
        // send(client_socket,"Enter password: ",strlen("Enter password: "),0);
        rec_client = recv(client_socket, &password_buffer, sizeof(password_buffer), 0);
        if (rec_client == -1)
        {
            perror("recv error");
            return false;
        }
        password_buffer[rec_client] = '\0';
        printf("emp 49 %s\n",password_buffer);
        // send(client_socket,"Enter user_id: ",strlen("Enter user_id: "),0);
        //char usrid_buff[1024];
        // int userid_buffer;
        rec_client = recv(client_socket, &userid_buffer, sizeof(userid_buffer), 0);
        printf("emp 54 %d\n",userid_buffer);
       // userid_buffer = atoi(usrid_buff);
        if (rec_client == -1)
        {
            perror("recv error");
            return false;
        }
        if (authenticate_employee(client_socket, userid_buffer, password_buffer))
        {
            printf("Inside emp auth 63\n");
            send(client_socket, "Authentication Successful", strlen("Authentication Successful"), 0);
            while (1)
            {
                // char menu_options[] = "Select action to perform(Select number corresponding to desired action):\n1)Add new customer\n2)Modify customer details\n3)Approve loan\n4)View Assigned Loan Applications\n5)View Customer Transactions(passbook)\n6)Change Password\n7)Logout\n8)Exit\n";
                // send(client_socket,menu_options,strlen(menu_options),0);
                // char menuopt_buff[1024];
                int client_rcv = recv(client_socket, &menu_options_recv, sizeof(menu_options_recv), 0);
                // menu_options_recv = atoi(menuopt_buff);
                int rec_cust_id;
                int customer_id;
                switch (menu_options_recv)
                {
                case 1:
                    add_customer(client_socket);
                    break;
                case 2:
                    edit_customer(client_socket);
                    break;
                case 3:
                    // send(client_socket, "Enter customer id whose loan you want to approve", strlen("Enter customer id whose loan you want to approve"), 0);
                    // int customer_id;
                    char rx_cust[1024];
                    rec_cust_id = recv(client_socket, rx_cust, sizeof(rx_cust), 0);
                    customer_id = atoi(rx_cust);
                    if (rec_cust_id == -1)
                    {
                        perror("recv error");
                        return false;
                    }
                    approve_loan(client_socket, userid_buffer, customer_id);
                    break;
                case 4:
                    //view_assigned_loan_applications(client_socket, userid_buffer);
                    break;
                case 5:
                    // send(client_socket, "Enter customer id: ", strlen("Enter customer id: "), 0);
                    // int customer_id;
                    char rx_idcust[1024];
                    // rec_cust_id = recv(client_socket, rx_idcust, sizeof(rx_idcust), 0);
                    // customer_id = atoi(rx_idcust);
                    // show_transaction_history(client_socket, customer_id);
                    break;
                case 6:
                    char *db = "employee_db.txt";
                    edit_credentials_employee(client_socket, userid_buffer);
                    break;
                case 7:
                    // logout:It will not terminate client server connection.
                    employee_logout(client_socket, userid_buffer);
                    break;
                case 8:
                    // exit: it will terminate client server connection.
                    send(client_socket, "Exiting..", strlen("Exiting.."), 0);
                    close(client_socket);
                    exit(0);
                default:
                    // send(client_socket, "Invalid response", strlen("Invalid response"), 0);
                    break;
                }
                if (menu_options_recv == 7)
                {
                    // user has logged out so come out of while loop here.
                    return 1;
                }
            }
        }
        else
        {
            printf("auth falied emp 132\n");
            send(client_socket, "Authentication failed", strlen("Authentication failed"), 0);
            return false;
        }
    }
    return true;
}
