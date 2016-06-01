#ifndef __SERVER_H
#define __SERVER_H
#define SOCK_REG 0x00
#define SOCK_SND 0x01
#define ERR_NOFD 0x02
#define NAME_MAX 16
#define MAGIC_SIZE 2
#define CMD_SIZE 1
#define RECV_MIN (NAME_MAX + CMD_SIZE + MAGIC_SIZE)
#define BUFF_SIZE 1024
#define SERVICE_PORT 5555
struct thread_data {
	struct usr_hash *hash;
	pthread_cond_t cond;
	pthread_mutex_t mutex;
	int	ready_count;
	int	user_count;
	int new_user_fd;
	int max_user;
};

#endif
