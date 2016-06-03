#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#include "log.h"
#include "command.h"

#define ARRAY_SIZE(a) (sizeof(a)/sizeof((a)[0]))

static int exec(int argc, char *argv, struct buffer *buff)
{
	return 0;
}

static struct command cmds[] = {
	{"help", exec},
	{"info", exec}
};

int main(int argc, char *argv[])
{
	int ret;

	ret = cmd_init(cmds, ARRAY_SIZE(cmds));
	printf("Hello, I am yunuo board, cmd init %d\n", ret);
	log_printf(LOG_DEBUG, "Hello, I am yunuo board, cmd init %d\n", ret);

	return 0;
}
