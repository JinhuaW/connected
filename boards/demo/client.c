#include <stdio.h>
#include <conu.h>
#include <string.h>

int main(int argc, char *argv[])
{
	int fd;
	MSG *msg = NULL;
	char data[BUFF_MAX];
	int n;
	fd = tp_reg(argv[0], "127.0.0.1");
	if (argv[1] && strcmp(argv[1], "to") == 0) {
		while (1) {
			fgets(data, BUFF_MAX, stdin);
			tp_send(fd, argv[2], data, strlen(data) + 1);
		}
	} else {
		msg = msg_malloc(0, NULL, BUFF_MAX);
		while (1) {
			n = tp_recv(fd, msg);
			printf("Recv %s(%d) from %s\n", (char *)msg->data, n, msg->name);
		}
	}
	close(fd);
}	

