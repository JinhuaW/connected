#ifndef __BUFFER_H__
#define __BUFFER_H__

struct buffer {
	char *data;
	int data_size;
	int buff_size;
};

#define buff_data(buff)       ((buff)->data)
#define buff_data_size(buff)  ((buff)->data_size)
#define buff_size(buff)       ((buff)->buff_size)

struct buffer *buff_new(int size);
void buff_free(struct buffer *buff);
int buff_reset(struct buffer *buff);

#endif
