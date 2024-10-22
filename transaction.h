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
#include "transactionHistory.h"
#ifndef TRANSACTION_H
#define TRANSACTION_H
// define view_balance in separate header file so that both transaction and transactionHistory.h can use it
// without circular dependecy.
//  int view_balance(int client_socket,int customer_id)
//  {
//  	//find customer in db then show balance.
//  		int fd_read = open("customer_db.txt",O_RDWR);
//  		int current_balance = 0;
//  		struct customer temp_cust;
//          while(read(fd_read,&temp_cust,sizeof(struct customer)) > 0)
//  		{
//  			if(customer_id == temp_cust.customer_id)
//  			{
//  				//found the customer now first take advisory lock then update balance.
//  				struct flock lk;
//  				lk.l_type = F_RDLCK;
//  				lk.l_whence = SEEK_CUR;
//  				lk.l_start = lseek(fd_read,0,SEEK_CUR) - sizeof(struct customer);
//  				lk.l_len = sizeof(struct customer);
//  				if(fcntl(fd_read,F_SETLKW,&lk) == -1)
//  				{
//  					perror("error fcntl");
//  					close(fd_read);
//  					close(client_socket);
//  					return -1;
//  				}
//  				current_balance = temp_cust.balance;
//  				lk.l_type = F_UNLCK;
//  				if(fcntl(fd_read,F_SETLK,&lk) == -1)
//  				{
//  					perror("error fcntl");
//  					close(fd_read);
//  					close(client_socket);
//  					return -1;
//  				}
//  				char msg[256];
//  				sprintf(msg,"Current balance: %d",current_balance);
//  				send(client_socket,msg,strlen(msg),0);
//  			}
//  			else
//  			{
//  				send(client_socket,"Could not find customer id in db",strlen("Could not find customer id in db"),0);
//  			}
//  			close(fd_read);
//  			//close(client_socket);
//  		}
//  		return current_balance;

// }
void deposit(int client_socket, int customer_id, int add_balance)
{
	// find customer_id in customer_db file then update balance.
	int fd_read = open("customer_db.txt", O_RDWR);
	struct customer temp_cust;
	while (read(fd_read, &temp_cust, sizeof(struct customer)) > 0)
	{
		if (customer_id == temp_cust.customer_id)
		{
			printf("inside deposit fucntion 74 line\n");
			// found the customer now first take advisory lock then update balance.
			struct flock lk;
			lk.l_type = F_WRLCK;
			lk.l_whence = SEEK_CUR;
			lk.l_start = lseek(fd_read, 0, SEEK_CUR) - sizeof(struct customer);
			lk.l_len = sizeof(struct customer);
			if (fcntl(fd_read, F_SETLKW, &lk) == -1)
			{
				perror("error fcntl");
				close(fd_read);
				// close(client_socket);
				return;
			}
			int old_balance = temp_cust.balance;
			temp_cust.balance = temp_cust.balance + add_balance;
			char *transaction_type = "Deposit: ";
			log_transaction(client_socket, customer_id, add_balance, transaction_type);
			// before unlocking write updated data in file db.
			if (lseek(fd_read, -sizeof(struct customer), SEEK_CUR) == -1)
			{
				perror("lseek error");
				return;
			}
			if ((write(fd_read, &temp_cust, sizeof(struct customer))) <= 0)
			{
				perror("write error");
				return;
			}
			lk.l_type = F_UNLCK;
			if (fcntl(fd_read, F_SETLK, &lk) == -1)
			{
				perror("error fcntl");
				close(fd_read);
				// close(client_socket);
				return;
			}
			char msg[256];
			sprintf(msg, "Balance updated.old balance: %d.New balance: %d", old_balance, temp_cust.balance);
			send(client_socket, msg, strlen(msg), 0);
			close(fd_read);
			return;
		}
	}
	send(client_socket, "Could not find customer id in db", strlen("Could not find customer id in db"), 0);
	close(fd_read);
}

void withdraw(int client_socket, int customer_id, int withdraw_amount)
{
	// find customer id in db,take lock then update balance.
	int fd_read = open("customer_db.txt", O_RDWR);
	struct customer temp_cust;
	while (read(fd_read, &temp_cust, sizeof(struct customer)) > 0)
	{
		if (customer_id == temp_cust.customer_id)
		{
			// found the customer now first take advisory lock then update balance.
			struct flock lk;
			lk.l_type = F_WRLCK;
			lk.l_whence = SEEK_CUR;
			lk.l_start = lseek(fd_read, 0, SEEK_CUR) - sizeof(struct customer);
			lk.l_len = sizeof(struct customer);
			if (fcntl(fd_read, F_SETLKW, &lk) == -1)
			{
				perror("error fcntl");
				close(fd_read);
				// close(client_socket);
				return;
			}
			// check if current balance is sufficient to withdraw.
			int old_balance = temp_cust.balance;
			if (old_balance < withdraw_amount)
			{
				char msg[256];
				sprintf(msg, "Not enough balance.Current balance: %d", old_balance);
				send(client_socket, msg, strlen(msg), 0);
				close(fd_read);
				// close(client_socket);
				return;
			}
			temp_cust.balance = temp_cust.balance - withdraw_amount;
			char *transaction_type = "Withdraw: ";
			log_transaction(client_socket, customer_id, withdraw_amount, transaction_type);
			// before unlocking write updated data in file db.
			if (lseek(fd_read, -sizeof(struct customer), SEEK_CUR) == -1)
			{
				perror("lseek error");
				return;
			}
			if ((write(fd_read, &temp_cust, sizeof(struct customer))) <= 0)
			{
				perror("write error");
				return;
			}
			lk.l_type = F_UNLCK;
			if (fcntl(fd_read, F_SETLK, &lk) == -1)
			{
				perror("error fcntl");
				close(fd_read);
				// close(client_socket);
				return;
			}
			char msg[256];
			sprintf(msg, "Balance updated.old balance: %d .New balance: %d", old_balance, temp_cust.balance);
			send(client_socket, msg, strlen(msg), 0);
			close(fd_read);
			return;
		}
	}
	send(client_socket, "Could not find customer id in db", strlen("Could not find customer id in db"), 0);
	close(fd_read);
}

void transfer_fund(int client_socket, int sender_customer_id, int receiver_customer_id, int transfer_amount)
{
	// find sender and receiver in cusotmer db file
	// check if sufficient balance is availabe then tranfer amount.
	int fd_read = open("customer_db.txt", O_RDWR);
	struct customer sender_id, receiver_id, temp;
	// use flags to check if both sender and receiver are present in db otherwise give error.
	bool sender_found = false;
	bool receiver_found = false;
	off_t sender_offset, receiver_offset;
	struct flock lks; // lock for sender
	struct flock lkr; // lock for receiver
	while (read(fd_read, &temp, sizeof(struct customer)) > 0)
	{
		if (temp.customer_id == sender_customer_id)
		{
			// as soon as we find sender take advisory lock as later in further iterations we'll lose the current
			// offset location of required struct customer.
			sender_id = temp;
			lks.l_type = F_WRLCK;
			lks.l_whence = SEEK_SET;
			lks.l_start = lseek(fd_read, 0, SEEK_CUR) - sizeof(struct customer);
			lks.l_len = sizeof(struct customer);
			if (fcntl(fd_read, F_SETLKW, &lks) == -1)
			{
				perror("fcntl error");
				close(fd_read);
				// close(client_socket);
				return;
			}
			sender_found = true;
			sender_offset = lseek(fd_read, 0, SEEK_CUR) - sizeof(struct customer);
			if (sender_offset == -1)
			{
				perror("lseek error");
				close(fd_read);
				return;
			}
		}
		if (temp.customer_id == receiver_customer_id)
		{
			receiver_id = temp;
			lkr.l_type = F_WRLCK;
			lkr.l_whence = SEEK_SET;
			lkr.l_start = lseek(fd_read, 0, SEEK_CUR) - sizeof(struct customer);
			lkr.l_len = sizeof(struct customer);
			if (fcntl(fd_read, F_SETLKW, &lkr) == -1)
			{
				perror("fcntl error");
				close(fd_read);
				// close(client_socket);
				return;
			}
			receiver_found = true;
			receiver_offset = lseek(fd_read, 0, SEEK_CUR) - sizeof(struct customer);
			if (receiver_offset == -1)
			{
				perror("lseek error");
				close(fd_read);
				return;
			}
		}
	}
	if (sender_found == true && receiver_found == true)
	{
		// both sender and receiver have been found and we've acquired locks,now we can proceed with fund transfer.
		if (sender_id.balance < transfer_amount)
		{
			char msg[256];
			sprintf(msg, "Not enough balance.Current balance: %d", sender_id.balance);
			send(client_socket, msg, strlen(msg), 0);
			close(fd_read);
			close(client_socket);
			return;
		}
		sender_id.balance = sender_id.balance - transfer_amount;
		receiver_id.balance = receiver_id.balance + transfer_amount;
		// before unlocking write updated data in file db.
		if (lseek(fd_read, sender_offset, SEEK_SET) == -1)
		{
			perror("lseek error");
			return;
		}
		if ((write(fd_read, &sender_id, sizeof(struct customer))) <= 0)
		{
			perror("write error");
			return;
		}
		// before unlocking write updated data in file db.
		if (lseek(fd_read, receiver_offset, SEEK_SET) == -1)
		{
			perror("lseek error");
			return;
		}
		if ((write(fd_read, &receiver_id, sizeof(struct customer))) <= 0)
		{
			perror("write error");
			return;
		}
		char transaction_type[256];
		sprintf(transaction_type, "Transferred amount %d to %d", transfer_amount, receiver_customer_id);
		log_transaction(client_socket, sender_customer_id, transfer_amount, transaction_type);
		sprintf(transaction_type, "Received amount %d from %d", transfer_amount, sender_customer_id);
		log_transaction(client_socket, receiver_customer_id, transfer_amount, transaction_type);
	}
	lks.l_type = F_UNLCK;
	lkr.l_type = F_UNLCK;
	if (fcntl(fd_read, F_SETLK, &lks) == -1)
	{
		perror("fcntl error");
		close(fd_read);
		// close(client_socket);
		return;
	}
	if (fcntl(fd_read, F_SETLK, &lkr) == -1)
	{
		perror("fcntl error");
		close(fd_read);
		// close(client_socket);
		return;
	}
	send(client_socket, "Amount transferred successfully\n", strlen("Amount transferred successfully\n"), 0);
	close(fd_read);
	// close(client_socket);
}
#endif