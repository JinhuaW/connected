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
	struct timeval timeout = {1, 0};
	fd_set read_set;
	FD_ZERO(&read_set);
	fd = tp_reg(argv[1], "127.0.0.1");
	if (fd <= 0)
		return -1;
	FD_SET(fd, &read_set);	
	msg = msg_malloc(0, NULL, BUFF_MAX);
	if (msg == NULL) {
		msg_free(msg);
		tp_exit(fd);
		return -1;
	}
	if (tp_send(fd, argv[2], argv[3], strlen(argv[3]) + 1) <= 0) {
		msg_free(msg);
		tp_exit(fd);
		return -1;
	}
	if (select(fd + 1, &read_set, NULL, NULL, &timeout) <= 0) {
		//printf("select failed\n");
		msg_free(msg);
		tp_exit(fd);
		return -1;
	} else {
		if (FD_ISSET(fd, &read_set)) {
			memset(msg, 0, sizeof(MSG));
			n = tp_recv(fd, msg);
			if (n > 0) {
				printf("%s\n", (char *)msg->data);
			} else {
				msg_free(msg);
				tp_exit(fd);
				return -1;
			}
		} else {
			//	printf("not read\n");
			msg_free(msg);
			tp_exit(fd);
			return -1;
		}
	}
	msg_free(msg);
	tp_exit(fd);
	return 0;
}	

