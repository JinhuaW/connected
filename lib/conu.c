#include "conu.h"
#include "hash.h"
#include "log.h"
#include "time_info.h"
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
	if (size < 0 || size > BUFF_MAX)
		return NULL;
	m_msg = (MSG *) malloc(msg_size);
	if (NULL == m_msg)
		return NULL;
	m_msg->magic[0] = 0xAA;
	m_msg->magic[1] = 0x55;
	if (NULL != name)
		memcpy(m_msg->name, name, NAME_MAX_LEN);
	m_msg->ctrl = ctrl;
	m_msg->data_size = htonl(size);
	m_msg->msg_size = htonl(msg_size);
	return m_msg;
}

void msg_update_name(MSG *msg, char *name)
{
	memcpy(msg->name, name, NAME_MAX_LEN);
}

int msg_update_size(MSG *msg, int size)
{
	if (size < 0 || size > BUFF_MAX)
		return -1;
	msg->msg_size = htonl(size + sizeof(MSG));
	msg->data_size = htonl(size);
	return 0;
}

void msg_free(MSG *msg)
{
	free(msg);
}

int tp_reg(char *name, char *server_ip)
{
	MSG *msg = msg_malloc(SOCK_LOGIN, name, 0);
	struct sockaddr_in server_addr;
	int sockfd;
	char buff[BUFF_MAX];
	char cur_time[BUFF_MAX];
	if (msg == NULL)
		return -1;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		goto exit2;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVICE_PORT);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);
	log_printf(LOG_DEBUG, "[%s]%s is connecting to server %s\n", get_cur_time(cur_time), name, server_ip );
	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
		goto exit1;
	
	log_printf(LOG_DEBUG, "[%s]%s is sending login info to server %s\n", get_cur_time(cur_time), name, server_ip );
	if (send(sockfd, msg, sizeof(MSG), 0) <= 0)
		goto exit1;
	log_printf(LOG_DEBUG, "[%s]%s is waiting the done info\n", get_cur_time(cur_time), name);
	if (recv(sockfd, buff, BUFF_MAX, 0) > 0 && strcmp(buff, "done") == 0) {
		msg->ctrl = SOCK_LOGIN_CONFIRM;
		log_printf(LOG_DEBUG, "[%s]%s is sending the confirm info\n", get_cur_time(cur_time), name);
		if (send(sockfd, msg, sizeof(MSG), 0) <= 0)
			goto exit1;
	} else {
		goto exit1;
	}
	usleep(200000);
	msg_free(msg);
	return sockfd;
exit1:
	close(sockfd);
exit2:
	msg_free(msg);
	return -1;
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
	memcpy(msg->data, data, size);
	ret = send(fd, msg, ntohl(msg->msg_size), 0);
	msg_free(msg);
	return ret;
}

int tp_recv(int fd, MSG *msg)
{
	int ret, recv_num = 0, try = 0;
	char cur_time[BUFF_MAX];
	if (msg == NULL)
		return -1;
	while (try++ < MAX_TRY && recv_num < sizeof(MSG)) {
		ret = recv(fd, msg + recv_num, sizeof(MSG) + BUFF_MAX, 0);
		if (ret <= 0) {
			if (ret == -1)
				log_printf(LOG_DEBUG, "[%s]Recv Error: %s\n", get_cur_time(cur_time), strerror(errno));
			else
				log_printf(LOG_DEBUG, "[%s]fd = %d is closing\n", get_cur_time(cur_time), fd);
			return -1;
		} else {
			recv_num += ret;
		}
	}
	while (try++ < MAX_TRY && recv_num < ntohl(msg->msg_size)) {
		ret = recv(fd, msg + recv_num, sizeof(MSG) + BUFF_MAX, 0);
		if (ret <= 0) {
			if (ret == -1)
				log_printf(LOG_DEBUG, "[%s]Recv Error: %s\n", get_cur_time(cur_time), strerror(errno));
			else
				log_printf(LOG_DEBUG, "[%s]fd = %d is closing\n", get_cur_time(cur_time), fd);
			
			return -1;
		} else {
			recv_num += ret;
		}
	}
	if (try >= MAX_TRY) {
		return -1;
	}
	if ((msg->magic[0] & 0xFF) != 0xAA || (msg->magic[1] & 0xFF) != 0x55) {
		log_printf(LOG_DEBUG, "[%s]unknown magic num(0x%02x,0x%02x)\n", get_cur_time(cur_time), msg->magic[0], msg->magic[1]);
		return 0;
	}
	return ntohl(msg->data_size);
}


//for server to tranport msg
static int tp_transfer(int fd, char *to, MSG *msg)
{
	msg_update_name(msg, to);
	return send(fd, msg, ntohl(msg->msg_size), 0);
}

//process msg
void conu_process(int sock_fd, struct usr_hash *hash)
{
	int recv_num, reg_flag = 0, recv_fd, login = 0;
	char from[NAME_MAX_LEN] = {}, to[NAME_MAX_LEN] = {}, cur_time[BUFF_MAX];
	MSG *msg = msg_malloc(0, NULL, BUFF_MAX);
	if (NULL == msg)
		return;
	memset(msg, 0, sizeof(MSG) + BUFF_MAX);
	while ( tp_recv(sock_fd, msg) >= 0 ) {
		switch (msg->ctrl) {
			case SOCK_LOGIN:
				memcpy(from, msg->name, NAME_MAX_LEN);
				log_printf(LOG_DEBUG, "[%s]new usr (%s) is trying to register\n", get_cur_time(cur_time), from);
				if (send(sock_fd, "done", 5, 0) != 5)
					goto exit;
				login = 1;
				break;
			case SOCK_LOGIN_CONFIRM:
				log_printf(LOG_DEBUG, "[%s] fd(%d) is recving confirm from %s, login = %d, reg_flag =%s, from = %s, name = %s \n", get_cur_time(cur_time), sock_fd,  msg->name, login, reg_flag, from, msg->name);
				if (login && !reg_flag && strcmp(from, msg->name) == 0) {
					if (hash_add_user(hash, from, sock_fd) == 0) {
						log_printf(LOG_DEBUG, "[%s]new usr (%s) registered\n", get_cur_time(cur_time), from);
						reg_flag = 1;
					} else {
						log_printf(LOG_DEBUG, "[%s]add (%s) to hash failed\n", get_cur_time(cur_time), from);
						goto exit;
					}
				} else {
					goto exit;
				}
				break;
			case SOCK_SND:
				if (reg_flag) {
					memcpy(to, msg->name, NAME_MAX_LEN);
					log_printf(LOG_DEBUG, "[%s]usr(%s) is sending \"%s\" to usr(%s)\n", get_cur_time(cur_time), from, msg->data, to);
					recv_fd = hash_get_fd_by_name(hash, to);
					if (recv_fd > 0)
						tp_transfer(recv_fd, from, msg);
				} else {
					goto exit;	
				}
				break;
			default:
				log_printf(LOG_DEBUG, "[%s]unknown ctrl command (0x%02x)\n", get_cur_time(cur_time), msg->ctrl);
				goto exit;
		}
		memset(msg, 0, sizeof(MSG) + BUFF_MAX);
	}
exit:
	msg_free(msg);
	close(sock_fd);
	if (reg_flag) {
		log_printf(LOG_DEBUG, "[%s]remove usr (%s) from hash list\n", get_cur_time(cur_time), from);
		hash_rm_user_by_name(hash, from);
	}
	return;
}

int new_server(int port, int max_user)
{
	struct sockaddr_in host_addr;
	int listen_sock = 0;
	char cur_time[BUFF_MAX];
	
	memset((void *)&host_addr, 0, sizeof(struct sockaddr_in));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(SERVICE_PORT);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0) {
		log_printf(LOG_CRIT, "[%s]create socked failed, %s\n", get_cur_time(cur_time), strerror(errno));
		return -1;
	}
	if (bind(listen_sock, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0) {
		log_printf(LOG_CRIT, "[%s]bind socket failed, %s\n", get_cur_time(cur_time), strerror(errno));
		return -1;
	}
	if (listen(listen_sock, max_user) < 0) {
		log_printf(LOG_CRIT, "[%s]listen socket failed, %s\n", get_cur_time(cur_time), strerror(errno));
		return -1;
	}
	return listen_sock;
}
