#include "hash.h"
#include <string.h>
#include <stdlib.h>

static int hash_get_tap(int usr_num)
{
	int tap = 0;
	int temp = 1; 
	int tap_max = sizeof(int) / sizeof(char);
	if (usr_num <= 0)
		return 0;
	while (temp < usr_num && tap++ < tap_max)
		temp = temp << 8;
	return tap;	
}

static int hash_calc(struct usr_hash *hash, char *name)
{
	int ret = 0;
	char *calc_hash = NULL;
	int index = 0;
	if (!hash)
		return -1;
	calc_hash = (char *)malloc(sizeof(char) * hash->tap);
	memset(calc_hash, 0xff, sizeof(char) * hash->tap);
	while (*name) {
		calc_hash[index++] &= *name;
		name++;
		index = index % hash->tap;
	}
	index = 0;
	while (index < hash->tap) {
		ret += calc_hash[index] << (index * 8);
		index++;
	}
	ret = ret % hash->max_usr;
	return ret;
}

struct usr_hash *hash_init(int usr_num)
{
	struct usr_hash *hash = NULL;
	if (usr_num <= 0)
		return NULL;
	hash = (struct usr_hash *)malloc(sizeof(struct usr_hash) + sizeof(struct usr_info) * (usr_num * 3 / 2));
	if (!hash)
		return NULL;
	hash->max_usr = usr_num;
	hash->tap = hash_get_tap(usr_num);
	if (hash->tap == 0) {
		free(hash);
		return NULL;
	}
	pthread_rwlock_init(&hash->rwlock, NULL);
	return hash;
}

void hash_destory(struct usr_hash *hash)
{
	int index = 0;
	if (!hash)
		return;
	pthread_rwlock_destroy(&hash->rwlock);
	free(hash);
	return;	
}

int hash_add_user(struct usr_hash *hash, char *name, int fd)
{
	int index, limit, hash_id = hash_calc(hash, name);
	if (hash_id < 0)
		return -1;
	index = hash_id;
	limit = (hash_id + hash->max_usr -1) % hash->max_usr;
	pthread_rwlock_wrlock(&hash->rwlock);	
	while (hash->usr[index].flag) {
		if (index == limit)
			return -1;
		index = (index + 1) % hash->max_usr;
	}
	hash->usr[index].flag = 1;
	hash->usr[index].fd = fd;
	memcpy(hash->usr[index].name, name, NAME_MAX_LEN);
	pthread_rwlock_unlock(&hash->rwlock);	
	return 0;
}

int hash_rm_user_by_name(struct usr_hash *hash, char *name)
{
	int index, limit, hash_id = hash_calc(hash, name);
	if (hash_id < 0)
		return -1;
	index = hash_id;
	limit = (hash_id + hash->max_usr -1) % hash->max_usr;
	pthread_rwlock_wrlock(&hash->rwlock);	
	while (strcmp(name, hash->usr[index].name) || strlen(name) != strlen(hash->usr[index].name)) {
		if (index == limit)
			return -1;
		index = (index + 1) % hash->max_usr;
	}
	hash->usr[index].flag = 0;
	memset(hash->usr[index].name, 0, NAME_MAX_LEN);
	pthread_rwlock_unlock(&hash->rwlock);	
	return 0;
}

int hash_get_fd_by_name(struct usr_hash *hash, char *name)
{
	
	int fd, index, limit, hash_id = hash_calc(hash, name);
	if (hash_id < 0)
		return -1;
	index = hash_id;
	limit = (hash_id + hash->max_usr -1) % hash->max_usr;
	pthread_rwlock_rdlock(&hash->rwlock);	
	while (strcmp(name, hash->usr[index].name) || strlen(name) != strlen(hash->usr[index].name)) {
		if (index == limit)
			return -1;
		index = (index + 1) % hash->max_usr;
	}
	fd = hash->usr[index].fd;
	pthread_rwlock_unlock(&hash->rwlock);
	return fd;	
}
