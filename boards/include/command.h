#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "buffer.h"

/* command interface defines
 * command format:
 * cmd arg1 arg2 arg3 ...
 * */

#define COMMAND_ARGS_MAX_NUM       16

struct command {
	const char *cmd;
	int (*exec) (int argc, char *argv[], struct buffer * buff);
};

int cmd_init(struct command *cmds, int cmds_nr);
int cmd_exec(char *cmdline, struct buffer *buff);

#endif
