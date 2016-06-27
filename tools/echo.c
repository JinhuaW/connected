#include <stdio.h>
#include <conu.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int fd;
	MSG *msg = NULL;
	char data[BUFF_MAX];
	int n;
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
			tp_exit(fd);
			return -1;
		}
		printf("recved from (%s): %s\n", msg->name, (char *)msg->data);
		if (tp_send(fd, msg->name, msg->data, strlen(msg->data) + 1) < 0) {
			printf("Failed to send the msg back to user(%s)\n", msg->name);
			tp_exit(fd);
			return -1;
		}
	}
	tp_exit(fd);
	return 0;
}	

