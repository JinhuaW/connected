#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>
#include <sys/wait.h>

#define BUFFER_SIZE 1024
#define  CR      0x0D
#define  LF      0x0A
#define  BS      0x08 /* Backspace character */
#define  ESC     0x1B /* Escape charackter */
#define  DEL     0x7f /* Delete, but on some terminals backspace */
#define FALSE 0
#define TRUE 1
#define ARG_MAX 10

void simple_printf(char *buf)
{
	fprintf(stdout,"%s", buf);
	fflush(stdout);
}

int str2list(char *buf, char *argv[], int argc_max)
{
	int count = 0;
	char *new, *next;
	if (NULL == buf)
		return 0;
	while (*buf && *buf == ' ')
		buf++;
	if (*buf) {
		new = buf;
		while (*new) {
			argv[count++] = new;
			if (count == argc_max)
				return 0;
			next = strchr(new, ' ');
			if (next) {
				*next = '\0';
				next++;
				new = next;
			} else {
				break;
			}
		}
	}
	argv[count] = NULL;
	return count;
}

static int ptym_open (const char *local_name)
{
	char *slave_name = NULL;
	int fd;
	struct termios termios;
	fd = open("/dev/ptmx",O_RDWR);
	if(fd < 0)
		return -1;
	slave_name = (char *)ptsname(fd);
	if(slave_name == NULL)
	{
		close(fd);
		return -1;
	}
	grantpt(fd);
	unlockpt(fd);
	remove(local_name);
	
	if (fcntl(fd, F_SETOWN, getpid()) < 0)
	{
		close(fd);
		return -1;
	}

	if (symlink(slave_name, local_name) < 0)
	{
		close(fd);
		return -1;
	}

	if (tcgetattr (fd, &termios) < 0) {
		close(fd);
		return -1;
	}

	/* Set raw mode on tty */
	cfmakeraw(&termios);

	if (tcsetattr (fd, TCSANOW, &termios) < 0) {
		close(fd);
		return -1;
	}
	return fd;
}

void shell(int std_in)
{
	unsigned char cmd[BUFFER_SIZE] = {0};
	int           len, status;
	unsigned char ch, eoc;
	char *argv[ARG_MAX];
	int	arg_count;
	int pid;
	simple_printf("\r\n");
	for (;;) {
		simple_printf(">");
		/* read the command line */
		len = 0;
		eoc = FALSE; /* set to not end-of-command */
		do {
			if (read(std_in, (char *)&ch, 1)>0) {
				switch(ch) {
					case BS:
					case DEL:
						if (len > 0) {
							len--;
							simple_printf("\b \b");
						}
						break;

					case CR:
					case LF:
						cmd[len] = '\0';
						eoc = TRUE;
						simple_printf("\r\n");
						break;

					default:
						/* is it a printable character */
						cmd [len] = ch;
						if ((ch >= 0x20) && (ch <= 0x7E) && (len < BUFFER_SIZE - 2)) {
							len++;
							putchar(ch);
							fflush(stdout);
						}
						break;

				} /* switch(ch) */
			}
		} while (!eoc); /* read till enf-of-command */

		if (cmd[0] != '\0') {
			if (strcmp(cmd, "exit") == 0)
				break;
			arg_count = str2list(cmd, argv, ARG_MAX);
			if (arg_count > 0) {
				pid = fork();
				if (pid < 0) {
					return;
				} else if (pid == 0) {
					execvp(argv[0], argv);
				} else {
					waitpid(pid, &status, 0);
				}
			}
		}
	}
}
/*
int main(int argc, char **argv)
{
	int fd, pid, status;
	fd = ptym_open("tty1_ext");	
	if (fd < 0) {
		printf("Failed to Open create the pseudo!");
		return 1;
	}
	pid = fork();
	if (pid < 0) {
		printf("Failed to fork!\n");
		return 1;
	} else if (pid == 0) {
		shell(fd);	
	} else {
		waitpid(pid, &status, 0);
		printf("Exit Shell!\n");
	}
	return 0;
}
*/
