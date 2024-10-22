#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "transactionHistory.h"
#include "customer.h"
#include "employee.h"
#include "manager.h"
#include "admin.h"
int main()
{

    char *server_address = "127.0.0.1"; // We're writing loopback address or local host addr bcz server is on same machine
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1)
    {
        perror("Socket error");
        exit(1);
    }

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(8080);
    inet_pton(AF_INET, server_address, &serverAddr.sin_addr);

    // Connect to the server
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        perror("Connection failed");
        close(clientSocket);
        exit(1);
    }

    printf("Connected to the server\n");
    while (1)
    {
        // Receive data from the server
        printf("Select Role(Enter number corresponding desired role: )\n1)Customer\n2)Employee\n3)Manager\n4)Admin\n");
        int menu_choice;
        char username[1024];
        char password[1024];
        int userid;
        scanf("%d", &menu_choice);
        send(clientSocket, &menu_choice, sizeof(menu_choice), 0);
        switch (menu_choice)
        {
        case 1:
            // char username[1024];
            // char password[1024];
            // int userid;
            printf("Enter username: \n");
            scanf("%s", username);
            send(clientSocket, username, strlen(username), 0);
            printf("Enter password: \n");
            scanf("%s", password);
            send(clientSocket, password, strlen(password), 0);
            printf("add customer client 59 %s \n", password);
            printf("Enter user_id: \n");
            scanf("%d", &userid);
            send(clientSocket, &userid, sizeof(userid), 0);
            char auth_buffer[1024];
            int auth_rcv = recv(clientSocket, auth_buffer, sizeof(auth_buffer), 0);
            auth_buffer[auth_rcv] = '\0';
            printf("%s\n", auth_buffer);
            if (strcmp("Authentication Successful", auth_buffer) == 0)
            {
                while (1)
                {
                    char menu_options[] = "Select action to perform(Select number corresponding to desired action):\n1)View Account Balance\n2)Deposit\n3)Withdraw\n4)Transfer Funds\n5)Apply For Loan\n6)Change Password\n7)Adding Feedback\n8)View Transaction History\n9)Logout\n10)Exit\n";

                    printf("%s", menu_options);
                    int user_choice;
                    scanf("%d", &user_choice);
                    send(clientSocket, &user_choice, sizeof(user_choice), 0);
                    switch (user_choice)
                    {
                    case 1:
                        char view_buff[1024];
                        int rec_view = recv(clientSocket, view_buff, sizeof(view_buff), 0);
                        view_buff[rec_view] = '\0';
                        printf("%s", view_buff);
                        break;
                    case 2:
                        printf("Enter amount you want to deposit: \n");
                        int deposit;
                        char deposit_buf[1024];
                        scanf("%d", &deposit);
                        send(clientSocket, &deposit, sizeof(deposit), 0);
                        int rec_deposit = recv(clientSocket, deposit_buf, sizeof(deposit_buf), 0);
                        deposit_buf[rec_deposit] = '\0';
                        printf("%s \n", deposit_buf);
                        break;
                    case 3:
                        printf("Enter amount to withdraw: \n");
                        int withdraw;
                        char withdraw_buff[1024];
                        scanf("%d", &withdraw);
                        send(clientSocket, &withdraw, sizeof(withdraw), 0);
                        int rec_withdraw = recv(clientSocket, withdraw_buff, sizeof(withdraw_buff), 0);
                        withdraw_buff[rec_withdraw] = '\0';
                        printf("%s \n", withdraw_buff);
                        break;
                    case 4:
                        printf("Enter receiver's id: \n");
                        int rec_id;
                        scanf("%d", &rec_id);
                        send(clientSocket, &rec_id, sizeof(rec_id), 0);
                        printf("Enter amount to transfer: \n");
                        int tr_amount;
                        scanf("%d", &tr_amount);
                        send(clientSocket, &tr_amount, sizeof(tr_amount), 0);
                        char tr_buffer[1024];
                        int rec_tr = recv(clientSocket, tr_buffer, sizeof(tr_buffer), 0);
                        tr_buffer[rec_tr] = '\0';
                        printf("%s\n", tr_buffer);
                        break;
                    case 5:
                        printf("Enter loan amount: \n");
                        int loan_amount;
                        char loan_buff[1024];
                        scanf("%d", &loan_amount);
                        send(clientSocket, &loan_amount, sizeof(loan_amount), 0);
                        int rec_loan = recv(clientSocket, loan_buff, sizeof(loan_buff), 0);
                        loan_buff[rec_loan] = '\0';
                        printf("%s\n", loan_buff);
                        break;
                    case 6:
                        edit_credentials_customer(userid);
                        break;
                    case 7:
                        printf("Enter feedback: \n");
                        char feed[256];
                        scanf("%s", feed);
                        send(clientSocket, feed, sizeof(feed), 0);
                        char feed_buff[1024];
                        int rec_feed = recv(clientSocket, feed_buff, sizeof(feed_buff), 0);
                        printf("%s\n", feed_buff);
                        break;
                    case 8:
                        show_transaction_history(userid);
                        break;
                    case 9:
                        char logout_buff[1024];
                        int rec_logout = recv(clientSocket, logout_buff, sizeof(logout_buff), 0);
                        logout_buff[rec_logout] = '\0';
                        printf("%s\n", logout_buff);
                        break;
                    case 10:
                        char exit_buff[256];
                        int rec_exit = recv(clientSocket, exit_buff, sizeof(exit_buff), 0);
                        exit_buff[rec_exit] = '\0';
                        close(clientSocket);
                        exit(0);
                    default:
                        printf("Invalid response\n");
                        break;
                    }
                    if (user_choice == 9)
                    {
                        break;
                    }
                }
            }
            else
            {
                // char rec_err[256];
                // int rec_err_ = recv(clientSocket, rec_err, sizeof(rec_err), 0);
                printf("%s\n", auth_buffer);
            }
            break;
        case 2:
            // handle employee
            printf("Enter username: \n");
            char username_buffer[128];
            char password_buffer[128];
            char buffer[1024];
            int userid_buffer;

            scanf("%s", username_buffer);
            send(clientSocket, username_buffer, strlen(username_buffer), 0);
            printf("Enter password: \n");
            scanf("%s", password_buffer);
            send(clientSocket, password_buffer, strlen(password_buffer), 0);
            printf("Enter user_id: \n");
            scanf("%d", &userid_buffer);
            send(clientSocket, &userid_buffer, sizeof(userid_buffer), 0);
            char auth[1000];
            int rcv_auth = recv(clientSocket, auth, sizeof(auth), 0);

            auth[rcv_auth] = '\0';
            printf("client 189 %s\n", auth);
            if (strcmp(auth, "Authentication Successful") == 0)
            {
                printf("Aithentication Successful. \n");
                int flag = 0;
                while (1)
                {
                    printf("Select action to perform(Select number corresponding to desired action):\n1)Add new customer\n2)Modify customer details\n3)Approve loan\n4)View Assigned Loan Applications\n5)View Customer Transactions(passbook)\n6)Change Password\n7)Logout\n8)Exit\n");
                    int menu;
                    scanf("%d", &menu);
                    send(clientSocket, &menu, sizeof(menu), 0);
                    switch (menu)
                    {
                    case 1:
                        // add new customer
                        printf("enter user name: \n");
                        char user_name[100];
                        scanf("%s", user_name);
                        send(clientSocket, user_name, strlen(user_name), 0);

                        printf("enter customerID: \n");
                        int id;
                        scanf("%d", &id);
                        send(clientSocket, &id, sizeof(id), 0);

                        printf("enter balance: \n");
                        int balance;
                        scanf("%d", &balance);
                        send(clientSocket, &balance, sizeof(balance), 0);

                        printf("Enter account number:\n");
                        int accno;
                        scanf("%d", &accno);
                        send(clientSocket, &accno, sizeof(accno), 0);

                        printf("Enter password for new customer: \n");
                        char password[100];
                        scanf("%s", password);
                        send(clientSocket, password, strlen(password), 0);

                        printf("Enter contact number for new customer:\n");
                        int contact;
                        scanf("%d", &contact);
                        send(clientSocket, &contact, sizeof(contact), 0);

                        printf("Enter address for new customer: \n");
                        char address[100];
                        scanf("%s", address);
                        send(clientSocket, address, strlen(address), 0);

                        int a;
                        recv(clientSocket, &a, sizeof(a), 0);

                        if (a == 0)
                        {
                            printf("customer id or account number already exists. \n");
                        }
                        else
                        {
                            printf("customer id or account number added successfully .\n");
                        }
                        break;
                    case 2:
                        // edit new customer.

                        printf("Enter customer id that you want to modify:\n");
                        int id_;
                        scanf("%d", &id_);
                        send(clientSocket, &id_, sizeof(id_), 0);

                        printf("Enter key to update the following characterstices \npress 1 for name. \npress 2 for contact. \npress 3 for address. \n");
                        int choice;
                        scanf("%d", &choice);
                        send(clientSocket, &choice, sizeof(choice), 0);
                        switch (choice)
                        {
                        case 1:
                            printf("enter new customer name: \n");
                            char name[256];
                            scanf("%s", name);
                            send(clientSocket, name, strlen(name), 0);
                            break;
                        case 2:
                            printf("enter new contact: \n");
                            int contact;
                            scanf("%d", &contact);
                            send(clientSocket, &contact, sizeof(contact), 0);
                            break;
                        case 3:
                            printf("Enter new address for customer: \n");
                            char address[100];
                            scanf("%s", address);
                            send(clientSocket, address, strlen(address), 0);
                            break;
                        default:
                            printf("Invalid input\n");
                            break;
                        }

                        char s[200];
                        recv(clientSocket, s, sizeof(s), 0);
                        if (strcmp(s, "record updated.") == 0)
                        {
                            printf("record updated.\n");
                        }
                        else
                        {
                            printf("user not updated.");
                        }
                        break;
                    case 3:
                        // loan approve;
                        printf("Enter customer id whose loan you want to approve\n");
                        int user;
                        scanf("%d", &user);
                        send(clientSocket, &user, sizeof(user), 0);
                        // char buffer[1024];
                        int rec = recv(clientSocket, buffer, sizeof(buffer), 0);
                        buffer[rec] = '\0';
                        printf("%s", buffer);
                        break;
                    case 4:
                        view_assigned_loan_applications(userid_buffer);
                        // char buffer[1024];
                        // int rec = recv(clientSocket, buffer, sizeof(buffer), 0);
                        // buffer[rec] = '\0';
                        // printf("%s", buffer);
                        break;
                    case 5:
                        printf("Enter customer id: \n");
                        int c_id;
                        scanf("%d", &c_id);
                        // send(clientSocket, &c_id, sizeof(c_id), 0);
                        show_transaction_history(c_id);
                        break;
                    case 6:
                        // char buffer[1024];
                        // int rec = recv(clientSocket, buffer, sizeof(buffer), 0);
                        // buffer[rec] = '\0';
                        // printf("%s", buffer);
                        printf("Enter new password:\n");
                        char buff[256];
                        scanf("%s", buff);
                        send(clientSocket, buff, strlen(buff), 0);
                        char s_[200];
                        int rcv = recv(clientSocket, s_, sizeof(s_), 0);
                        s_[rcv] = '\0';
                        printf("%s", s_);
                        break;
                    case 7:
                        // char buffer[1024];
                        int rec_ = recv(clientSocket, buffer, sizeof(buffer), 0);
                        buffer[rec_] = '\0';
                        printf("%s", buffer);
                        break;
                    case 8:
                        char buffer_[1024];
                        int rec1 = recv(clientSocket, buffer_, sizeof(buffer_), 0);
                        buffer_[rec1] = '\0';
                        printf("%s", buffer_);
                        // flag = 1;
                        break;
                    default:
                        // char buffer[1024];
                        //  int rec2 = recv(clientSocket, buffer, sizeof(buffer), 0);
                        //  buffer[rec2] = '\0';
                        //  printf("%s", buffer);
                        //  flag = 1;
                        break;
                    }
                    if (menu == 7)
                        break;
                }
            }
            else
            {
                // char buffer[1024];
                // int rec3 = recv(clientSocket, buffer, sizeof(buffer), 0);
                // buffer[rec3] = '\0';
                printf("%s", auth);
            }
            break;

        case 3:
            // for manager.
            // char username[1024];
            // char password[1024];
            // int userid;
            printf("Enter username: \n");
            scanf("%s", username);
            send(clientSocket, username, strlen(username), 0);
            printf("Enter password: \n");
            scanf("%s", password);
            send(clientSocket, password, strlen(password), 0);
            printf("Enter user_id: \n");
            scanf("%d", &userid);
            send(clientSocket, &userid, sizeof(userid), 0);
            char auth_buffer_[1024];
            int auth_rcv_ = recv(clientSocket, &auth_buffer_, sizeof(auth_buffer_), 0);
            auth_buffer_[auth_rcv_] = '\0';
            if (authenticate_manager(clientSocket, userid, password))
            {
                printf("Select action to perform(Select number corresponding to desired action):\n1)Deactivate account\n2)Show all applied loan applications\n3)Assign loan application to employee\n4)Review Feedback\n5)Change password\n6)logout\n7)exit\n");
                int choice;
                scanf("%d", &choice);
                send(clientSocket, &choice, sizeof(choice), 0);
                switch (choice)
                {
                case 1:
                    printf("Enter customer id that you want to deactivate:\n");
                    int cust_id;
                    scanf("%d", &cust_id);
                    send(clientSocket, &cust_id, sizeof(cust_id), 0);
                    break;
                case 2:
                    break;
                case 3:
                    printf("Enter employee id to whom you want to assign loan application: \n");
                    int emp_id_loan;
                    scanf("%d", &emp_id_loan);
                    send(clientSocket, &emp_id_loan, sizeof(emp_id_loan), 0);
                    printf("Enter customer id whose loan you want to assign: \n");
                    int cust_id_loan;
                    scanf("%d", &cust_id_loan);
                    send(clientSocket, &cust_id_loan, sizeof(cust_id_loan), 0);
                    char loan_buff[256];
                    int rec_loan = recv(clientSocket, loan_buff, sizeof(loan_buff), 0);
                    printf("%s", loan_buff);
                    break;
                case 4:
                    printf("Enter customer id whose feedback you want to resolve.\n");
                    int cust_id_feed;
                    scanf("%d", &cust_id_feed);
                    resolve_feedback(cust_id_feed);
                    break;
                case 5:
                    printf("Enter new password:\n");
                    char new_pass[256];
                    scanf("%s", new_pass);
                    send(clientSocket, new_pass, sizeof(new_pass), 0);
                    char pass_buff[1024];
                    int rec_new_pass = recv(clientSocket, pass_buff, sizeof(pass_buff), 0);
                    printf("%s", pass_buff);
                    break;
                case 6:
                    char logout_manager_buff[1024];
                    int rec_logout_ = recv(clientSocket, logout_manager_buff, sizeof(logout_manager_buff), 0);
                    printf("%s", logout_manager_buff);
                    break;
                case 7:
                    break;
                default:
                    char rec_def_buff[1024];
                    int rec_def = recv(clientSocket, rec_def_buff, sizeof(rec_def_buff), 0);
                    printf("%s", rec_def_buff);
                    break;
                }
                if (choice == 6)
                    break;
            }
            else
            {
                char rec_err[256];
                int rec_err_ = recv(clientSocket, rec_err, sizeof(rec_err), 0);
                printf("%s", rec_err);
            }
            break;
        case 4:
            // admin.
            printf("Enter username: \n");
            char name[256];
            scanf("%s", name);
            send(clientSocket, name, strlen(name), 0);

            printf("Enter password: \n");
            char pass[256];
            scanf("%s", pass);
            send(clientSocket, pass, strlen(pass), 0);

            printf("Enter user_id: \n");
            int id;
            scanf("%d", &id);
            send(clientSocket, &id, sizeof(id), 0);

            // char buff[256];
            char auth_buff[32];
            int rec_auth_buff = recv(clientSocket, auth_buff, sizeof(auth_buff), 0);
            auth_buff[rec_auth_buff] = '\0';
            printf("recv by client 471 %s\n", auth_buff);
            if (rec_auth_buff == -1)
            {
                perror("recv erro admin 472");
                return 1;
            }
            if (strcmp("admin authenticated", auth_buff) == 0)
            {
                // int rcv = recv(clientSocket, buff, sizeof(buff), 0);
                // buff[rcv] = '\0';
                printf("Authentication successful\n");
                while (1)
                {
                    printf("Select action to perform(Select number corresponding to desired action):\n1)Add New Bank Employee\n2)Modify Customer\n3)Modify Employee\n4)Manage User Roles\n5)Change password\n6)logout\n7)exit\n");
                    int choice;
                    scanf("%d", &choice);
                    printf("selected choice: %d", choice);
                    send(clientSocket, &choice, sizeof(choice), 0);
                    // char test_buff[32];
                    // int rec_test = recv(clientSocket,&test_buff,sizeof(test_buff),0);
                    // printf("%s",test_buff);
                    // test:
                    // char test_rec_buff[32];
                    // int test_rec = recv(clientSocket,test_rec_buff,sizeof(test_rec_buff),0);
                    // test_rec_buff[test_rec] = '\0';

                    // printf("test received at client: %d",atoi(test_rec_buff));
                    switch (choice)
                    {
                    case 1:
                        printf("enter user name: \n");
                        char name[256];
                        scanf("%s", name);
                        send(clientSocket, name, strlen(name), 0);

                        printf("Enter employee id: \n");
                        int id;
                        scanf("%d", &id);
                        send(clientSocket, &id, sizeof(id), 0);

                        printf("Enter password for new employee:\n");
                        char pass[256];
                        scanf("%s", pass);
                        send(clientSocket, pass, strlen(pass), 0);

                        char buff[256];
                        int rcv = recv(clientSocket, buff, sizeof(buff), 0);
                        buff[rcv] = '\0';
                        if (strcmp(buff, "employee already exists.") == 0)
                        {
                            printf(" employee already exists.\n");
                        }
                        else
                        {
                            printf("employee added \n");
                        }
                        break;
                    case 2:
                        printf("Enter customer id that you want to modify:\n");
                        int id2;
                        scanf("%d", &id2);
                        send(clientSocket, &id2, sizeof(id2), 0);

                        printf("Enter key to update the following characterstices \npress 1 for name. \npress 2 for contact. \npress 3 for address. \n");
                        int choice;
                        scanf("%d", &choice);
                        send(clientSocket, &choice, sizeof(choice), 0);
                        switch (choice)
                        {
                        case 1:
                            printf("enter new customer name: \n");
                            char name[256];
                            scanf("%s", name);
                            send(clientSocket, name, strlen(name), 0);
                            break;
                        case 2:
                            printf("enter new contact: \n");
                            int contact;
                            scanf("%d", &contact);
                            send(clientSocket, &contact, sizeof(contact), 0);
                            break;
                        case 3:
                            printf("Enter new address for customer: \n");
                            char address[100];
                            scanf("%s", address);
                            send(clientSocket, address, strlen(address), 0);
                            break;
                        default:
                            printf("Invalid input\n");
                            break;
                        }

                        char s[200];
                        int rcv2 = recv(clientSocket, s, sizeof(s), 0);
                        s[rcv2] = '\0';
                        if (strcmp(s, "record updated.") == 0)
                        {
                            printf("record updated.\n");
                        }
                        else
                        {
                            printf("user not updated.\n");
                        }
                        break;
                    case 3:
                        printf("Enter employee id that you want to modify:\n");
                        int id3;
                        scanf("%d", &id3);
                        send(clientSocket, &id3, sizeof(id3), 0);

                        printf("Enter employee name: \n");
                        char buff2[256];
                        scanf("%s", buff2);
                        send(clientSocket, buff2, strlen(buff2), 0);
                        break;
                    case 4:
                        printf("Enter employee id that you want to make manager.\n");
                        int id5;
                        scanf("%d", &id5);
                        send(clientSocket, &id5, sizeof(id5), 0);

                        char s2[200];
                        int rcv5 = recv(clientSocket, s2, sizeof(s2), 0);
                        s2[rcv5] = '\0';
                        printf("%s", s2);
                        break;
                    case 5:
                        printf("Enter new password:\n");
                        char buff3[256];
                        scanf("%s", buff3);
                        send(clientSocket, buff3, strlen(buff3), 0);
                        char s3[200];
                        int rcv6 = recv(clientSocket, s3, sizeof(s3), 0);
                        s3[rcv6] = '\0';
                        printf("%s", s3);
                        break;
                    case 6:
                        char s4[200];
                        int rcv7 = recv(clientSocket, s4, sizeof(s4), 0);
                        s4[rcv7] = '\0';
                        printf("%s", s4);
                        break;
                    case 7:
                        char s8[200];
                        int rcv8 = recv(clientSocket, s8, sizeof(s8), 0);
                        s8[rcv8] = '\0';
                        printf("%s", s8);
                        break;
                    default:
                        printf("enter valid argument \n");
                        break;
                    }
                    if (choice == 6)
                        break;
                }
            }
            else
            {
                // char s[200];
                // int rcv = recv(clientSocket, s, sizeof(s), 0);
                // s[rcv] = '\0';
                printf("%s", auth_buff);
                break;
            }
        default:
            break;
        }
    }
    // Close the socket
    close(clientSocket);
    return 0;
}