#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "buffer.h"

struct buffer *buff_new(int size)
{
	struct buffer *buff;

	if (size <= 0)
		return NULL;

	buff = malloc(sizeof(*buff) + size);
	if (buff) {
		buff->buff_size = size;
		buff->data_size = 0;
		buff->data = (char *) &buff[1];
	}

	return buff;
}

void buff_free(struct buffer *buff)
{
	if (buff)
		free(buff);
}

int buff_reset(struct buffer *buff)
{
	if (!buff)
		return -EINVAL;
	buff->data_size = 0;
	buff->data = (char *) &buff[1];
	return 0;
}
