#ifndef __HASH_H
#define __HASH_H
#include <pthread.h>

#define NAME_MAX 16
#define SERVICE_PORT 5555
struct usr_info {
	char name[NAME_MAX];
	int fd;
	int flag;
};

struct usr_hash {
	int max_usr;
	int tap;
	pthread_rwlock_t rwlock;
	struct usr_info usr[0];
};
struct usr_hash *hash_init(int usr_num);
void hash_destory(struct usr_hash *hash);
int hash_add_user(struct usr_hash *hash, char *name, int fd);
int hash_rm_user_by_name(struct usr_hash *hash, char *name);
int hash_get_fd_by_name(struct usr_hash *hash, char *name);

#endif
