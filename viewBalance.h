

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
#ifndef VIEWBALANCE_H
#define VIEWBALANCE_H
int view_balance(int client_socket,int customer_id)
{
	//find customer in db then show balance.
		int fd_read = open("customer_db.txt",O_RDWR);
		int current_balance = 0;
		struct customer temp_cust;
        while(read(fd_read,&temp_cust,sizeof(struct customer)) > 0)
		{
			if(customer_id == temp_cust.customer_id)
			{
				//found the customer now first take advisory lock then update balance.
				struct flock lk;
				lk.l_type = F_RDLCK;
				lk.l_whence = SEEK_CUR;
				lk.l_start = lseek(fd_read,0,SEEK_CUR) - sizeof(struct customer);
				lk.l_len = sizeof(struct customer);
				if(fcntl(fd_read,F_SETLKW,&lk) == -1)
				{
					perror("error fcntl");
					close(fd_read);
					close(client_socket);
					return -1;
				}
				current_balance = temp_cust.balance;
				lk.l_type = F_UNLCK;
				if(fcntl(fd_read,F_SETLK,&lk) == -1)
				{
					perror("error fcntl");
					close(fd_read);
					close(client_socket);
					return -1;
				}
				char msg[256];
				sprintf(msg,"Current balance: %d",current_balance);
				send(client_socket,msg,strlen(msg),0);
				close(fd_read);
				return current_balance;
			}
		}
		send(client_socket,"Could not find customer.\n",strlen("Could not find customer.\n"),0);
		return current_balance;

}
#endif