#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__
#include "hash.h"
#include <stdint.h>

#define SOCK_LOGIN 0x00
#define SOCK_LOGIN_CONFIRM 0x01
#define SOCK_SND 0x02
#define ERR_NOFD 0x03

//feed back code
#define SND_SUCCESS 0xF0
#define SND_OFFLINE 0xF1

#define MAGIC_SIZE 2
#define CMD_SIZE 1
#define BUFF_MAX 1024


#define ERR_CREATE	-1
#define ERR_CONNECT -2
#define ERR_SEND	-3
#define MAX_TRY 5
typedef struct msg {
	char magic[2];
	char ctrl;
	char name[NAME_MAX_LEN];
	uint32_t msg_size;
	uint32_t data_size;
	char data[0];
} __attribute__((packed)) MSG;


MSG *msg_malloc(char ctrl, char *name, int size);
void msg_free(MSG *msg);

//for client
int tp_reg(char *name, char *server_ip);
int tp_exit(int fd);
int tp_send(int fd, char *to, void *msg, int size);
int tp_recv(int fd, MSG *msg);

//for server
void conu_process(int sock_fd, struct usr_hash *hash);
int new_server(int port, int max_user);

int msg_update_size(MSG *msg, int size);
void msg_update_name(MSG *msg, char *name);
MSG *msg_malloc(char ctrl, char *name, int size);
void msg_free(MSG *msg);
#endif
