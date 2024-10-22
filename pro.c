#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "credentials.h"
#include <openssl/evp.h>
#include <openssl/sha.h>
int main()
{
	int fd = open("admin_db.txt",O_RDWR);
	struct admin temp;
	temp.emp_id = 1;
	strcpy(temp.username,"admin1");
	char psd[128] = "pswd";
	char hashed_pswd[32];
	char hex_pswd[64];
	char msg[128];
	
	printf("original:%s",psd);
	hash_password(psd, hashed_pswd);
	strcpy(msg,hashed_pswd);
	printf("hashed:%s",msg);
	password_hash_to_hex(hashed_pswd,hex_pswd);
	strcpy(temp.password,hex_pswd);
	printf("hex password:%s",temp.password);
	temp.is_online = false;
	if(write(fd,&temp,sizeof(struct admin))<=0)
	{
		perror("write error");
		return 1;
	}
	return 0;
}


