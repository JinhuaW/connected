#include <stdio.h>
#include <conu.h>
#include <string.h>
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
	int fd;
	MSG *msg = NULL;
	char data[BUFF_MAX];
	int n, ret;
	struct timeval timeout = {0, 300000};
	fd_set read_set;
	FD_ZERO(&read_set);
	fd = tp_reg(argv[1], "127.0.0.1");
	if (fd <= 0)
		return -1;
	FD_SET(fd, &read_set);	
	msg = msg_malloc(0, NULL, BUFF_MAX);
	if (msg == NULL) {
		tp_exit(fd);
		return -1;
	}
	usleep(10);
	if (tp_send(fd, argv[2], argv[3], strlen(argv[3]) + 1) <= 0) {
		tp_exit(fd);
		return -1;
	}
	if (select(fd + 1, &read_set, NULL, NULL, &timeout) <= 0) {
//		printf("select failed\n");
		tp_exit(fd);
		return -1;
	} else {
		if (FD_ISSET(fd, &read_set)) {
			memset(msg, 0, sizeof(MSG));
			n = tp_recv(fd, msg);
			printf("%s\n", (char *)msg->data);
		} else {
//			printf("not read\n");
			tp_exit(fd);
			return -1;
		}
	}
	tp_exit(fd);
	return 0;
}	

