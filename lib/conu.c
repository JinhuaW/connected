#include "conu.h"
#include "hash.h"
#include "log.h"
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <syslog.h>
#include <errno.h>  
#include <netinet/in.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

MSG *msg_malloc(char ctrl, char *name, int size)
{
	MSG *m_msg = NULL;	
	int msg_size = sizeof(MSG) + size;
	if (size < 0)
		return NULL;
	m_msg = (MSG *) malloc(msg_size);
	if (NULL == m_msg)
		return NULL;
	m_msg->magic[0] = 0xAA;
	m_msg->magic[1] = 0x55;
	if (NULL != name)
		memcpy(m_msg->name, name, NAME_MAX);
	m_msg->ctrl = ctrl;
	m_msg->data_size = size;
	m_msg->msg_size = msg_size;
	return m_msg;
}

void msg_update_name(MSG *msg, char *name)
{
	memcpy(msg->name, name, NAME_MAX);
}

void msg_update_size(MSG *msg, int size)
{
	msg->msg_size = size;
	msg->data_size = size - sizeof(MSG);
}

void msg_free(MSG *msg)
{
	free(msg);
}

int tp_reg(char *name, char *server_ip)
{
	MSG *msg = msg_malloc(SOCK_REG, name, 0);
	struct sockaddr_in server_addr;
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		msg_free(msg);
		return ERR_CREATE;
	}
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVICE_PORT);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		msg_free(msg);
		close(sockfd);
		return ERR_CONNECT;
	}

	if (send(sockfd, msg, msg->msg_size, 0) <= 0) {
		msg_free(msg);
		close(sockfd);
		return ERR_SEND;
	}
	msg_free(msg);
	return sockfd;
}

int tp_exit(int fd)
{
	close(fd);
}

int tp_send(int fd, char *to, void *data, int size)
{
	int ret = 0;
	MSG *msg = msg_malloc(SOCK_SND, to, size);
	if (NULL == msg)
		return -1;
	if (msg->data_size > BUFF_MAX) {
		msg_free(msg);
		return -1;
	}
	memcpy(msg->data, data, size);
	ret = send(fd, msg, msg->msg_size, 0);
	msg_free(msg);
	return ret;
}

int tp_recv(int fd, MSG *msg)
{
	int ret;
	if (msg == NULL)
		return -1;
	ret = recv(fd, msg, msg->msg_size, 0);
	if (ret < sizeof(MSG))
		return -1;
	msg->data_size = ret - sizeof(MSG);
	return msg->data_size;
}


//for server to tranport msg
static int tp_transfer(int fd, char *to, MSG *msg)
{
	msg_update_name(msg, to);
	return send(fd, msg, msg->msg_size, 0);
}

//process msg
void conu_process(int sock_fd, struct usr_hash *hash)
{
	int recv_num, reg_flag = 0, recv_fd;
	char from[NAME_MAX] = {}, to[NAME_MAX] = {};
	MSG *msg = msg_malloc(0, NULL, BUFF_MAX);
	if (NULL == msg)
		return;
	while (1) {
		recv_num = recv(sock_fd, msg, sizeof(MSG) + BUFF_MAX, 0);
		if (recv_num == 0) {
			if (reg_flag)
				hash_rm_user_by_name(hash, from);
			close(sock_fd);
			break;
		} else {
			if (recv_num < sizeof(MSG))
				return;

			if (msg->magic[0] != 0xAA || msg->magic[1] != 0x55)
				return;

			switch (msg->ctrl) {
			case SOCK_REG:
				memcpy(from, msg->name, NAME_MAX);
				hash_add_user(hash, from, sock_fd);
				reg_flag = 1;
				break;
			case SOCK_SND:
				if (recv_num != msg->msg_size)
					break;	
				msg_update_size(msg, recv_num);
				memcpy(to, msg->name, NAME_MAX);
				recv_fd = hash_get_fd_by_name(hash, to);
				if (tp_transfer(recv_fd, from, msg) < 0) {
					msg_free(msg);
					return;
				}
				break;
			default:
				return;
				break;
			}
		}
	}
	msg_free(msg);
	return;
}

int new_server(int port, int max_user)
{
	struct sockaddr_in host_addr;
	int listen_sock = 0;

	memset((void *)&host_addr, 0, sizeof(struct sockaddr_in));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(SERVICE_PORT);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0) {
		log_printf(LOG_CRIT, "%s %d:create socked failed, %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	if (bind(listen_sock, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0) {
		log_printf(LOG_CRIT, "%s %d:bind socket failed, %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	if (listen(listen_sock, max_user) < 0) {
		log_printf(LOG_CRIT, "%s %d:listen socket failed, %s\n", __FILE__, __LINE__, strerror(errno));
		return -1;
	}
	return listen_sock;
}
