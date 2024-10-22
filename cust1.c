#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "credentials.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
char name[128]; // this name will be used as username.
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
int main()
{
    int fd = open("customer_db.txt", O_RDWR);
    struct customer temp;
    temp.customer_id = 1;
    temp.is_active = true;
    temp.account_number = 1234;
    strcpy(temp.name, "cust1");
    char psd[128] = "pswd";
    char hashed_pswd[32];
    char hex_pswd[64];
    char msg[128];

    printf("original:%s", psd);
    hash_password(psd, hashed_pswd);
    // strcpy(msg, hashed_pswd);
    //  printf("hashed:%s", msg);
    password_hash_to_hex(hashed_pswd, hex_pswd);
    strcpy(temp.password, hex_pswd);
    // printf("hex password:%s", temp.password);
    temp.is_online = false;
    temp.balance = 1000;
    temp.need_loan = false;
    temp.loan_approved = false;
    temp.loan_amount = 0;
    temp.contact = 1234;
    char buff[20] = "iiitb";
    strcpy(temp.address, buff);
    if (write(fd, &temp, sizeof(struct admin)) <= 0)
    {
        perror("write error");
        return 1;
    }
    return 0;
}