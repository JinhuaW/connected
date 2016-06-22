#include <stdio.h>
#include <conu.h>
#include <string.h>
#include <unistd.h>


int main(int argc, char *argv[])
{
	int fd;
	MSG *msg = NULL;
	char data[BUFF_MAX];
	int n;
	fd = tp_reg(argv[1], "127.0.0.1");
	if (fd <= 0)
		return -1;	
	msg = msg_malloc(0, NULL, BUFF_MAX);
	if (msg == NULL)
		return -1;
	usleep(10);
	if (tp_send(fd, argv[2], argv[3], strlen(argv[3]) + 1) <= 0)
		return -1;
	memset(msg, 0, sizeof(MSG));
	n = tp_recv(fd, msg);
	printf("%s\n", (char *)msg->data);
	tp_exit(fd);
	return 0;
}	

