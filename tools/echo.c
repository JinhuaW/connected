#include <stdio.h>
#include <conu.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int fd;
	MSG *msg = NULL;
	char data[BUFF_MAX];
	int n;
	fd = tp_reg(argv[1], "127.0.0.1");
	msg = msg_malloc(0, NULL, BUFF_MAX);
	while (1) {
		memset(msg, 0, sizeof(MSG));
		tp_recv(fd, msg);
		printf("recved from (%s): %s\n", msg->name, (char *)msg->data);
		tp_send(fd, msg->name, msg->data, strlen(msg->data) + 1);
	}
	tp_exit(fd);
}	

