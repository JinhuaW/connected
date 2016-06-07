#ifndef __TRANSPORT_H__
#define __TRANSPORT_H__
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>  
#include <netinet/in.h> 

#define SOCK_REG 0x00
#define SOCK_SND 0x01
#define ERR_NOFD 0x02



//feed back code
#define SND_SUCCESS 0xF0
#define SND_OFFLINE 0xF1




#define NAME_MAX 16
#define MAGIC_SIZE 2
#define CMD_SIZE 1

#define BUFF_MIN (NAME_MAX + CMD_SIZE + MAGIC_SIZE)
#define BUFF_MAX 1024


#define ERR_CREATE	-1
#define ERR_CONNECT -2
#define ERR_SEND	-3

//for client
int conu_reg(char *name, char *server_ip);
int conu_exit(int fd);
int conu_send(int fd, char *to, void *msg, int size);
int conu_recv(int fd, char *from, void *msg);

//for server
void conu_process(int sock_fd, struct usr_hash *hash);
#endif
