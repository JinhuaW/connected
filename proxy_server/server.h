#ifndef __SERVER_H
#define __SERVER_H

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
