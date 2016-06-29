#include <stdio.h>
#include <conu.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define ARGC_MAX 10

int main(int argc, char *argv[])
{
	int fd, n, pid, arg_count, status, count;
	MSG *msg = NULL;
	char *exec_argv[ARGC_MAX], *data;
	FILE *fp;	
	if (argc != 3) {
		printf("Usage: %s usr_name server_ip\n", argv[0]);
		return -1;
	}
	fd = tp_reg(argv[1], argv[2]);
	if (fd <= 0) {
		printf("Failed to connect to server, usr_name = %s, server_ip = %s\n", argv[1], argv[2]);
		return -1;
	}	
	msg = msg_malloc(0, NULL, BUFF_MAX);
	if (msg == NULL) {
		printf("Failed to malloc msg!\n");
		tp_exit(fd);
		return -1;
	}
	while (1) {
		memset(msg, 0, sizeof(MSG) + BUFF_MAX);
		if (tp_recv(fd, msg) <= 0) {
			printf("Failed to recv msg\n");
			msg_free(msg);
			tp_exit(fd);
			return -1;
		}
		printf("recved from (%s): %s\n", msg->name, (char *)msg->data);
		fp = popen((char *)msg->data, "r");
		if (fp == NULL) {
			continue;
		} else {
			count = 0;
			data = msg->data;
			while(!feof(fp) && count < BUFF_MAX) {
				n = fread(data, sizeof(char), BUFF_MAX - count, fp);
				data += n;
				count += n;
			}
			msg_update_size(msg, count);
			pclose(fp);
		}
		if (count <= BUFF_MAX )
			tp_send(fd, msg->name, msg->data, strlen(msg->data) + 1);
	}
	msg_free(msg);
	tp_exit(fd);
	return 0;
}	

