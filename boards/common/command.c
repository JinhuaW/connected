#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "command.h"

static struct command *this_cmds = NULL;
static int this_cmds_nr = 0;

static int cmd_string_to_args(char *string, char **argv)
{
	int argc = 0;
	//TODO
	return argc;
}

static struct command *cmd_find(char *cmd)
{
	//TODO
	return 0;
}

int cmd_init(struct command *cmds, int cmds_nr)
{
	if (!cmds || cmds_nr <= 0)
		return -EINVAL;
	this_cmds = cmds;
	this_cmds_nr = cmds_nr;
	return 0;
}

int cmd_exec(char *cmdline, struct buffer *buffer)
{
	struct command *cmd;
	int argc;
	char *argv[COMMAND_ARGS_MAX_NUM + 1] = { 0 };

	if (!cmdline || strlen(cmdline) <= 0)
		return -EINVAL;

	argc = cmd_string_to_args(cmdline, argv);
	if (argc <= 0)
		return -EINVAL;

	cmd = cmd_find(argv[0]);
	if (!cmd)
		return -EINVAL;

	return cmd->exec(argc, argv, buffer);
}
