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
bool handle_customer(int client_socket);

bool handle_customer(int client_socket)
{
	while (1)
	{
		// first authenticate then proceed.
		// send(client_socket, "Enter username: ", strlen("Enter username: "), 0);
		printf("Inside customer\n");
		char username_buffer[128];
		char password_buffer[128];
		int userid_buffer;
		int menu_options_recv;
		int rec_client = recv(client_socket, &username_buffer, sizeof(username_buffer), 0);
		username_buffer[rec_client] = '\0';
		printf("handle_customer 34 |%s|\n", username_buffer);

		if (rec_client == -1)
		{
			perror("recv error");
			return false;
		}
		// send(client_socket, "Enter password: ", strlen("Enter password: "), 0);
		rec_client = recv(client_socket, &password_buffer, sizeof(password_buffer), 0);

		if (rec_client == -1)
		{
			perror("recv error");
			return false;
		}
		password_buffer[rec_client] = '\0';
		printf("handle_customer 43 |%s|\n", password_buffer);
		// send(client_socket, "Enter user_id: ", strlen("Enter user_id: "), 0);
		// char userid_buff[1024];
		rec_client = recv(client_socket, &userid_buffer, sizeof(userid_buffer), 0);
		printf("custumer_handle 53 |%d|\n", userid_buffer);
		// userid_buffer = atoi(userid_buff);
		if (rec_client == -1)
		{
			perror("recv error");
			return false;
		}
		if (authenticate_customer(client_socket, userid_buffer, password_buffer))
		{
			// if authentication is successful.
			send(client_socket, "Authentication Successful", strlen("Authentication Successful"), 0);
			while (1)
			{
				// char menu_options[] = "Select action to perform(Select number corresponding to desired action):\n1)View Account Balance\n2)Deposit\n3)Withdraw\n4)Transfer Funds\n5)Apply For Loan\n6)Change Password\n7)Adding Feedback\n8)View Transaction History\n9)Logout\n10)Exit\n";
				// send(client_socket,menu_options,strlen(menu_options),0);

				int client_rcv = recv(client_socket, &menu_options_recv, sizeof(int), 0);
				switch (menu_options_recv)
				{
				case 1:
					view_balance(client_socket, userid_buffer);
					break;
				case 2:
					// send(client_socket, "Enter amount to deposit: ", strlen("Enter amount to deposit: "), 0);
					int amount;
					// char amount_buff[1024];
					rec_client = recv(client_socket, &amount, sizeof(amount), 0);
					printf("depsoit amt 82 customer: %d\n", amount);
					// amount = atoi(amount_buff);
					if (rec_client == -1)
					{
						perror("recv error");
						return false;
					}
					deposit(client_socket, userid_buffer, amount);
					break;
				case 3:
					int withdraw_amount;
					// send(client_socket, "Enter amount to withdraw: ", strlen("Enter amount to withdraw: "), 0);
					// char withdraw_buff[1024];
					rec_client = recv(client_socket, &withdraw_amount, sizeof(withdraw_amount), 0);
					printf("csutomer withdraw: %d\n", withdraw_amount);
					// withdraw_amount = atoi(withdraw_buff);
					if (rec_client == -1)
					{
						perror("recv error");
						return false;
					}
					withdraw(client_socket, userid_buffer, withdraw_amount);
					break;
				case 4:
					// send(client_socket, "Enter receiver's id: ", strlen("Enter receiver's id: "), 0);
					int receiver_customer_id;
					// char rx_buff[1024];
					rec_client = recv(client_socket, &receiver_customer_id, sizeof(receiver_customer_id), 0);
					// receiver_customer_id = atoi(rx_buff);
					//  send(client_socket, "Enter amount to transfer: ", strlen("Enter amount to transfer: "), 0);
					if (rec_client == -1)
					{
						perror("recv error");
						return false;
					}
					int transfer_amount;
					// char rx_amount_buff[1024];
					rec_client = recv(client_socket, &transfer_amount, sizeof(transfer_amount), 0);
					// transfer_amount = atoi(rx_amount_buff);
					if (rec_client == -1)
					{
						perror("recv error");
						return false;
					}
					transfer_fund(client_socket, userid_buffer, receiver_customer_id, transfer_amount);
					break;
				case 5:
					// send(client_socket, "Enter loan amount: ", strlen("Enter loan amount: "), 0);
					int loan_amount;
					// char loan_amount_buff[1024];
					int rec_loan = recv(client_socket, &loan_amount, sizeof(loan_amount), 0);
					// loan_amount = atoi(loan_amount_buff);
					if (rec_loan == -1)
					{
						perror("recv error");
						return false;
					}
					apply_loan(client_socket, userid_buffer, loan_amount);
					break;
				case 6:
					// char *db = "customer";
					// edit_credentials_customer(userid_buffer);
					break;
				case 7:
					char feedback[1024];
					// send(client_socket, "Enter feedback: ", strlen("Enter feedback: "), 0);
					rec_client = recv(client_socket, &feedback, sizeof(feedback), 0);
					if (rec_client == -1)
					{
						perror("recv error");
						return false;
					}
					feedback[rec_client] = '\0';
					add_feedback(client_socket, userid_buffer, feedback);
					// send(client_socket, "Feedback added successfully.", strlen("Feedback added successfully."), 0);
					break;
				case 8:
					// show_transaction_history(userid_buffer);
					break;
				case 9:
					// logout:It will not terminate client server connection.
					customer_logout(client_socket, userid_buffer);
					break;
				case 10:
					// exit: it will terminate client server connection.
					send(client_socket, "Exiting..", strlen("Exiting.."), 0);
					close(client_socket);
					exit(0);
				default:
					send(client_socket, "Invalid response", strlen("Invalid response"), 0);
					break;
				}
				if (menu_options_recv == 9)
				{
					// user has logged out so come out of while loop here.
					return 1;
				}
			}
		}
		else
		{
			// authentication failed.
			send(client_socket, "Username or Password is invalid", strlen("Username or Password is invalid"), 0);
			return false;
			// close(client_socket);
		}
	}
	return true;
}
