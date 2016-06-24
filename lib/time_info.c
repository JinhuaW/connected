#include <time.h>
#include <string.h>
#include <time_info.h>

char *get_cur_time(char *cur_time)
{
	time_t rawtime;
	struct tm * timeinfo;
	char *end;
	if (cur_time == NULL)
		return NULL;
	time(&rawtime);
	timeinfo = localtime ( &rawtime );
	strcpy(cur_time, asctime(timeinfo));
	end=strchr(cur_time, '\n');
	if (end)
		*end = '\0';
	return cur_time;
}
