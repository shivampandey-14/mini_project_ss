#include<stdbool.h>
#ifndef COMMON_STRUCT_H
#define COMMON_STRUCT_H

//id and account number are unique.
struct customer
{
	char name[128]; //this name will be used as username.
	int customer_id;
	int account_number;
	int balance;
	bool need_loan;
	int loan_amount;
	bool loan_approved;
	char password[128];
	bool is_online;
	bool is_active;
	int contact;
	char address[1024];
};

struct employee
{
	int emp_id;
	char username[128];
	char password[128];
	bool is_online;
	bool is_active; //this will be used when we want to change user role from emp to manager.
};

struct loan
{
	int emp_id;
	int customer_id; //customer id of customer whose loan app has been assigned to this emp.
	int loan_amount;
	char status[64]; //status could be applied,approved.

};
struct manager
{
	int emp_id;
	char username[128];
	char password[128];
	bool is_online;
};

struct admin
{
	int emp_id;
	char username[128];
	char password[128];
	bool is_online;
};

struct transact
{
	int user_id;
	char transaction[256];
	char date_time[128];
	int current_balance;
};
// struct cred
// {
// 	char user[128];
// 	char password[128];
// 	bool is_online;
// };
struct feedback
{
	int user_id;
	bool resolved;
	char feed_back[1024];
};

#endif  // COMMON_STRUCT_H