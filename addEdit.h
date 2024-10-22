#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/file.h>
#include <sys/socket.h>
#include "commonStruct.h"
#include "credentials.h"
#ifndef ADDEDIT_H
#define ADDEDIT_H
void add_customer(int client_socket)
{
	// first add new cusotmer details in the struct customer.then save it in db.
	// account
	struct customer new_customer;
	// send(client_socket,"enter user name: ",strlen("enter user name: "),0);
	int rcv_name = recv(client_socket, new_customer.name, sizeof(new_customer.name), 0);
	if (rcv_name <= 0)
	{
		perror("rcv error");
		return;
	}

	new_customer.name[rcv_name] = '\0';
	printf("addedit 30 |%s|\n", new_customer.name);
	int cust_id_buff;
	// send(client_socket,"enter customerID: ",strlen("enter customerID: "),0);
	int rcv_cusid = recv(client_socket, &cust_id_buff, sizeof(cust_id_buff), 0);
	new_customer.customer_id = cust_id_buff;
	printf("addedit id 35 |%d|\n", new_customer.customer_id);
	if (rcv_cusid <= 0)
	{
		perror("rcv error");
		return;
	}

	// send(client_socket,"enter balance: ",strlen("enter balance: "),0);
	int balance_buff;
	int rcv_balance = recv(client_socket, &balance_buff, sizeof(balance_buff), 0);
	new_customer.balance = balance_buff;
	printf("addedit id 46 %d\n", new_customer.balance);
	if (rcv_balance <= 0)
	{
		perror("rcv error");
		return;
	}
	// send(client_socket,"Enter account number:",strlen("Enter account number:"),0);
	int acc_buff;
	int rcv_account_number = recv(client_socket, &acc_buff, sizeof(acc_buff), 0);
	new_customer.account_number = acc_buff;
	printf("addedit id 56 |%d|\n", new_customer.account_number);
	if (rcv_account_number <= 0)
	{
		perror("rcv error");
		return;
	}
	// send(client_socket,"Enter password for new customer:",strlen("Enter password for new customer:"),0);
	char temp_pswd[128];
	int rcv_password = recv(client_socket, temp_pswd, sizeof(temp_pswd), 0);
	if (rcv_password <= 0)
	{
		perror("recv error");
		return;
	}
	temp_pswd[rcv_password] = '\0';
	printf("add edit 71 |%s|\n", temp_pswd);
	char hashed_pswd[32];
	char hex_pswd[65];
	trim_trailing_spaces(temp_pswd);
	trim_leading_spaces(temp_pswd);
	hash_password(temp_pswd, hashed_pswd);
	password_hash_to_hex(hashed_pswd, hex_pswd);
	strcpy(new_customer.password, hex_pswd);
	// send(client_socket,"Enter contact number for new customer:",strlen("Enter contact number for new customer:"),0);
	int contact_num;
	// char contact_buff[1024];
	int rcv_contact = recv(client_socket, &contact_num, sizeof(contact_num), 0);
	// contact_num = atoi(contact_buff);
	if (rcv_contact == -1)
	{
		perror("recv error");
		return;
	}
	new_customer.contact = contact_num;
	printf("add edit 89 %d\n", new_customer.contact);
	char addr[1024];
	// send(client_socket,"Enter address for new customer:",strlen("Enter address for new customer:"),0);
	int rcv_addr = recv(client_socket, addr, sizeof(addr), 0);
	if (rcv_addr == -1)
	{
		perror("recv error");
		return;
	}
	addr[rcv_addr] = '\0';
	strcpy(new_customer.address, addr);
	new_customer.need_loan = false;
	new_customer.loan_amount = 0;
	new_customer.loan_approved = false;
	new_customer.is_online = false;
	new_customer.is_active = true;
	// while savinf we'll also check if customer id and acc number already exists in db.if yes then send error.
	int fd_read = open("customer_db.txt", O_RDWR);
	if (fd_read == -1)
	{
		perror("error while opening customer db 110\n");
	}
	struct customer temp_cust;
	int flag = 0;
	while ((read(fd_read, &temp_cust, sizeof(struct customer))) > 0)
	{
		if (temp_cust.customer_id == new_customer.customer_id || temp_cust.account_number == new_customer.account_number)
		{
			send(client_socket, &flag, sizeof(flag), 0);
			// send(client_socket, "customer id or account number already exists.", strlen("customer id or account number already exists."), 0);
			close(fd_read);
			return;
		}
	}
	flag = 1;
	printf("file open susscefully customer_db 125\n");
	// add new cutomer to db after applying lock to whole file
	struct flock lk;
	lk.l_type = F_WRLCK;
	lk.l_whence = SEEK_END;
	lk.l_start = 0;
	lk.l_len = sizeof(struct customer);
	if (fcntl(fd_read, F_SETLKW, &lk) == -1)
	{
		perror("locking error");
		close(fd_read);
		// close(client_socket);
		return;
	}
	if (lseek(fd_read, 0, SEEK_END) == -1)
	{
		perror("lseek error 141 add edit");
		close(fd_read);
		return;
	}
	if ((write(fd_read, &new_customer, sizeof(struct customer))) <= 0)
	{
		perror("write error");
		close(fd_read);
		return;
	}
	// send(client_socket,"new customer added successfully",strlen("new customer added successfully"),0);
	send(client_socket, &flag, sizeof(flag), 0);
	lk.l_type = F_UNLCK;
	if (fcntl(fd_read, F_SETLK, &lk) == -1)
	{
		perror("locking error");
		close(fd_read);
		// close(client_socket);
		return;
	}
	close(fd_read);
}

void edit_customer(int client_socket)
{
	int user_id;
	// send(client_socket, "Enter customer id that you want to modify:", strlen("Enter customer id that you want to modify:"), 0);
	char emp_buff[1024];
	int rec_usr_id = recv(client_socket, emp_buff, sizeof(emp_buff), 0);
	user_id = atoi(emp_buff);
	if (rec_usr_id == -1)
	{
		perror("recv error");
		return;
	}
	struct customer new_customer;
	int menue_option = 0;
	char choice_buff[1024];
	// send(client_socket, "Enter key to update the following characterstices \npress 1 for name. \npress 2 for contact. \npress 3 for address. ", strlen("Enter key to update the following characterstices \npress 1 for name. \npress 2 for contact. \npress 3 for address. "), 0);
	int rcv_cusid = recv(client_socket, choice_buff, sizeof(choice_buff), 0);
	menue_option = atoi(choice_buff);
	switch (menue_option)
	{
	case 1:
		// send(client_socket, "enter new customer name: ", strlen("enter new customer name: "), 0);
		int rcv_name = recv(client_socket, new_customer.name, sizeof(new_customer.name), 0);
		if (rcv_name <= 0)
		{
			perror("rcv error");
			return;
		}
		new_customer.name[rcv_name] = '\0';
		break;
	case 2:
		// send(client_socket, "enter new contact: ", strlen("enter new contact: "), 0);
		char contact_buff[1024];
		int rcv_contact = recv(client_socket, contact_buff, sizeof(contact_buff), 0);
		new_customer.contact = atoi(contact_buff);
		if (rcv_contact <= 0)
		{
			perror("rcv error");
			return;
		}
		break;
	case 3:
		// send(client_socket, "enter new address: ", strlen("enter new address: "), 0);
		int rcv_address = recv(client_socket, new_customer.address, sizeof(new_customer.address), 0);
		if (rcv_address <= 0)
		{
			perror("rcv error");
			return;
		}
		new_customer.address[rcv_name] = '\0';
		break;
	default:
		send(client_socket, "Invalid input", strlen("Invalid input"), 0);
		break;
	}
	int fd = open("customer_db.txt", O_RDWR);
	if (fd < 0)
	{
		perror("error in opening the customer file");
		return;
	}
	struct customer buff;
	while ((read(fd, &buff, sizeof(buff))) > 0)
	{
		if (buff.customer_id == user_id)
		{
			if (menue_option == 1)
			{
				// send(client_socket, "customer found.updating reacord ....", strlen("customer found.updating reacord ...."), 0);
				struct flock lk;
				lk.l_start = lseek(fd, 0, SEEK_CUR) - sizeof(struct customer);
				lk.l_len = sizeof(struct customer);
				lk.l_type = F_WRLCK; // write lock,exclusive lock
				lk.l_whence = SEEK_SET;
				if (fcntl(fd, F_SETLKW, &lk) == -1)
				{
					perror("failed to get lock");
					return;
				}
				strcpy(buff.name, new_customer.name);
				if (lseek(fd, -sizeof(struct customer), SEEK_CUR) == -1)
				{
					perror("lseek error");
					close(fd);
					return;
				}
				if (write(fd, &buff, sizeof(buff)) < 0)
				{
					perror("write error");
					close(fd);
					return;
				}
				lk.l_type = F_UNLCK;
				if (fcntl(fd, F_SETLK, &lk) == -1)
				{
					perror("error releasing lock");
					return;
				}
				send(client_socket, "record updated.", strlen("record updated."), 0);
			}
			else if (menue_option == 2)
			{
				// send(client_socket, "customer found.updating reacord ....", strlen("customer found.updating reacord ...."), 0);
				struct flock lk;
				lk.l_start = lseek(fd, 0, SEEK_CUR) - sizeof(struct customer);
				lk.l_len = sizeof(struct customer);
				lk.l_type = F_WRLCK; // write lock,exclusive lock
				lk.l_whence = SEEK_SET;
				if (fcntl(fd, F_SETLKW, &lk) == -1)
				{
					perror("failed to get lock");
					return;
				}
				if (lseek(fd, -sizeof(struct customer), SEEK_CUR) == -1)
				{
					perror("lseek error");
					close(fd);
					return;
				}
				buff.contact = new_customer.contact;
				if (write(fd, &buff, sizeof(buff)) < 0)
				{
					perror("write error");
					close(fd);
					return;
				}
				lk.l_type = F_UNLCK;
				if (fcntl(fd, F_SETLK, &lk) == -1)
				{
					perror("error releasing lock");
					return;
				}
				send(client_socket, "record updated.", strlen("record updated."), 0);
			}
			else
			{
				// send(client_socket, "customer found.updating reacord ....", strlen("customer found.updating reacord ...."), 0);
				struct flock lk;
				lk.l_start = lseek(fd, 0, SEEK_CUR) - sizeof(struct customer);
				lk.l_len = sizeof(struct customer);
				lk.l_type = F_WRLCK; // write lock,exclusive lock
				lk.l_whence = SEEK_SET;
				if (fcntl(fd, F_SETLKW, &lk) == -1)
				{
					perror("failed to get lock");
					return;
				}
				if (lseek(fd, -sizeof(struct customer), SEEK_CUR) == -1)
				{
					perror("lseek error");
					close(fd);
					return;
				}
				strcpy(buff.address, new_customer.address);
				if (write(fd, &buff, sizeof(buff)) < 0)
				{
					perror("write error");
					close(fd);
					return;
				}
				lk.l_type = F_UNLCK;
				if (fcntl(fd, F_SETLK, &lk) == -1)
				{
					perror("error releasing lock");
					return;
				}
				send(client_socket, "record updated.", strlen("record updated."), 0);
			}
			return;
		}
	}
	send(client_socket, "Record not found", sizeof("Record not found"), 0);
}

void add_employee(int client_socket)
{
	printf("inside employee\n");
	struct employee new_emp;
	// send(client_socket, "enter user name: ", strlen("enter user name: "), 0);
	int rcv_name = recv(client_socket, &new_emp.username, sizeof(new_emp.username), 0);
	if (rcv_name <= 0)
	{
		perror("rcv error");
		return;
	}
	new_emp.username[rcv_name] = '\0';

	// send(client_socket, "Enter employee id: ", strlen("Enter employee id: "), 0);
	// char emp_id_buffer[1024];
	int rcv_empid = recv(client_socket, &new_emp.emp_id, sizeof(new_emp.emp_id), 0);
	// new_emp.emp_id = atoi(emp_id_buffer);
	if (rcv_empid <= 0)
	{
		perror("error in recv");
		close(client_socket);
		return;
	}
	// new_emp.loan_id = 0;
	new_emp.is_online = false;
	new_emp.is_active = true;
	// send(client_socket, "Enter password for new employee:", strlen("Enter password for new employee:"), 0);
	char temp_pswd[512];
	int rcv_password = recv(client_socket, &temp_pswd, sizeof(temp_pswd), 0);
	if (rcv_password <= 0)
	{
		perror("recv error");
		return;
	}
	temp_pswd[rcv_password] = '\0';

	char hashed_pswd[32];
	char hex_pswd[65];
	trim_trailing_spaces(temp_pswd);
	trim_leading_spaces(temp_pswd);
	hash_password(temp_pswd, hashed_pswd);
	password_hash_to_hex(hashed_pswd, hex_pswd);
	// this is for testing
	// char msg[1024];
	// snprintf(msg, sizeof(msg), "original:%s|", temp_pswd);
	// send(client_socket, msg, strlen(msg), 0);
	strcpy(new_emp.password, hex_pswd);
	int fd_read = open("employee_db.txt", O_RDWR);
	struct employee temp_emp;
	while ((read(fd_read, &temp_emp, sizeof(struct employee))) > 0)
	{
		if (temp_emp.emp_id == new_emp.emp_id)
		{
			// this is for testing
			// char msg[1024];
			// memset(msg, 0, sizeof(msg));
			// sprintf(msg, "old: %d new: %d", temp_emp.emp_id, new_emp.emp_id);
			// send(client_socket, msg, strlen(msg), 0);

			send(client_socket, "employee already exists.", strlen("employee already exists."), 0);
			close(fd_read);
			return;
		}
	}
	// add new employee to db.
	if (lseek(fd_read, 0, SEEK_END) == -1)
	{
		perror("lseek error");
		close(fd_read);
		return;
	}
	if (write(fd_read, &new_emp, sizeof(struct employee)) <= 0)
	{
		perror("write error");
		close(fd_read);
		return;
	}
	send(client_socket, "new employee added successfully", strlen("new employee added successfully"), 0);
}

void edit_employee(int client_socket)
{
	int emp_id;
	char emp_id_buff[1024];
	// send(client_socket, "Enter employee id that you want to modify:", strlen("Enter employee id that you want to modify:"), 0);
	int rcv_emp_id = recv(client_socket, &emp_id_buff, sizeof(emp_id_buff), 0);
	emp_id = atoi(emp_id_buff);
	if (rcv_emp_id == -1)
	{
		perror("recv error");
		return;
	}
	struct employee new_emp;
	// send(client_socket, "Enter employee name: ", strlen("Enter employee name: "), 0);
	int rcv_empid = recv(client_socket, &new_emp.username, sizeof(new_emp.username), 0);
	if (rcv_empid <= 0)
	{
		perror("error in recv");
		close(client_socket);
		return;
	}
	new_emp.username[rcv_empid] = '\0';
	int fd_read = open("employee_db.txt", O_RDWR);
	struct employee temp_emp;
	while ((read(fd_read, &temp_emp, sizeof(struct employee))) > 0)
	{
		if (temp_emp.emp_id == emp_id)
		{
			// acquire lock then update employee in db
			struct flock lk;
			lk.l_type = F_WRLCK;
			lk.l_whence = SEEK_SET;
			lk.l_start = lseek(fd_read, 0, SEEK_CUR) - sizeof(struct employee);
			lk.l_len = sizeof(struct employee);
			if (fcntl(fd_read, F_SETLKW, &lk) == -1)
			{
				perror("fcntl error");
				close(fd_read);
				return;
			}
			if (lseek(fd_read, -sizeof(struct employee), SEEK_CUR) < 0)
			{
				perror("lseek error");
				close(fd_read);
				return;
			}
			strcpy(temp_emp.username, new_emp.username);
			if (write(fd_read, &temp_emp, sizeof(temp_emp)) < 0)
			{
				perror("write error");
				close(fd_read);
				return;
			}
			lk.l_type = F_UNLCK;
			if (fcntl(fd_read, F_SETLK, &lk) == -1)
			{
				perror("fcntl error");
				close(fd_read);
				return;
			}
			// send(client_socket, "employee updated", strlen("employee updated"), 0);
		}
	}
	close(fd_read);
}

void deactivate_customer(int customer_id)
{
	// we'll not delete customer details from db,just deactivate account in struct customer.
	int fd_cust = open("customer_db.txt", O_RDWR);
	if (fd_cust == -1)
	{
		perror("open error");
		return;
	}
	struct customer temp_cust;
	while (read(fd_cust, &temp_cust, sizeof(struct customer)) > 0)
	{
		if (temp_cust.customer_id == customer_id)
		{
			// found customer in db.
			struct flock lk;
			lk.l_start = lseek(fd_cust, 0, SEEK_CUR) - sizeof(struct customer);
			lk.l_len = sizeof(struct customer);
			lk.l_type = F_WRLCK; // write lock,exclusive lock
			lk.l_whence = SEEK_CUR;
			if (fcntl(fd_cust, F_SETLKW, &lk) == -1)
			{
				perror("failed to get lock");
				return;
			}
			// now deactivate customer account.
			temp_cust.is_active = false;
			// release lock.
			lk.l_type = F_UNLCK;
			if (fcntl(fd_cust, F_SETLK, &lk) == -1)
			{
				perror("failed to release lock");
				return;
			}
			break;
		}
	}
}

void manage_user_role(int client_socket, int employee_id)
{
	// this will be used by admin to chnage user role from emp to manager.
	// first make is_active of emp false then add_manager using all fields of emp.
	int fd_emp = open("employee_db.txt", O_RDWR);
	if (fd_emp == -1)
	{
		perror("open error");
		return;
	}
	struct employee temp_emp;
	// deactivating existing emp data in db.
	while ((read(fd_emp, &temp_emp, sizeof(struct employee))) > 0)
	{
		if (temp_emp.emp_id == employee_id)
		{
			temp_emp.is_active = false;
			// write to employee db.
			if (lseek(fd_emp, -sizeof(struct employee), SEEK_CUR) == -1)
			{
				perror("lseek error");
				close(fd_emp);
				return;
			}
			if ((write(fd_emp, &temp_emp, sizeof(struct employee))) <= 0)
			{
				perror("write error");
				close(fd_emp);
				return;
			}
			break;
		}
	}
	// adding above emp data as manager data in manager_db.
	struct manager new_manager;
	new_manager.emp_id = temp_emp.emp_id;
	strcpy(new_manager.username, temp_emp.username);
	strcpy(new_manager.password, temp_emp.password);
	new_manager.is_online = false;
	// create new record in manager_db.
	int fd_manager = open("manager_db.txt", O_RDWR);
	if (fd_manager == -1)
	{
		perror("open error");
		close(fd_emp);
		return;
	}
	// take advisory lock at end of file as we're inserting new data at end.
	struct flock lk;
	lk.l_type = F_WRLCK;
	lk.l_whence = SEEK_END;
	lk.l_start = 0;
	lk.l_len = sizeof(struct manager);
	if (fcntl(fd_manager, F_SETLKW, &lk) == -1)
	{
		perror("fcntl error");
		close(fd_emp);
		close(fd_manager);
		return;
	}
	if ((write(fd_manager, &new_manager, sizeof(struct manager))) <= 0)
	{
		perror("write error");
		close(fd_emp);
		close(fd_manager);
		return;
	}
	lk.l_type = F_UNLCK;
	if (fcntl(fd_manager, F_SETLK, &lk) == -1)
	{
		perror("fcntl error");
		close(fd_emp);
		close(fd_manager);
		return;
	}
	char msg[256];
	sprintf(msg, "Employee '%s' is now manager.", temp_emp.username);
	send(client_socket, msg, strlen(msg), 0);
	close(fd_emp);
	close(fd_manager);
}
#endif
