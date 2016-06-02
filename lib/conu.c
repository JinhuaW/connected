#include "conu.h"
#include "hash.h"

int tp_reg(char *name, char *server_ip)
{
	char buf[BUFF_MAX] = {0xAA, 0x55, SOCK_REG};
	struct sockaddr_in server_addr;
	int sockfd;
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return ERR_CREATE;

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(SERVICE_PORT);
	inet_pton(AF_INET, server_ip, &server_addr.sin_addr);

	if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
		close(sockfd);
		return ERR_CONNECT;
	}

	memcpy(&buf[MAGIC_SIZE + CMD_SIZE], name, NAME_MAX);  
	if (send(sockfd, buf, BUFF_MIN, 0) <= 0) {
		close(sockfd);
		return ERR_SEND;
	}
	return 0;
}

int tp_exit(int fd)
{
	close(fd);
}

int tp_send(int fd, char *to, void *msg, int size)
{
	char buf[BUFF_MAX] = {0xAA, 0x55,SOCK_SND};
	if (size + BUFF_MIN > BUFF_MAX)
		return -1;
	memcpy(&buf[MAGIC_SIZE + CMD_SIZE], to, NAME_MAX);
	memcpy(&buf[BUFF_MIN], msg, size);
	return send(fd, buf, size + BUFF_MIN, 0);
}

int tp_recv(int fd, char *from, void *msg)
{
	char buf[BUFF_MAX];
	int ret;
	if (from == NULL || msg == NULL)
		return -1;
	ret = recv(fd, buf, BUFF_MAX, 0);
	if (ret < BUFF_MIN)
		return -1;
	memcpy(msg, &buf[BUFF_MIN], ret - BUFF_MIN);
	return ret - BUFF_MIN;
}


//for server to tranport msg
static int tp_transfer(int fd, char *to, void *buf, int size)
{
	memcpy(&buf[MAGIC_SIZE + CMD_SIZE], to, NAME_MAX);
	return send(fd, buf, size, 0);
}

//process msg
void conu_process(int sock_fd, struct usr_hash *hash)
{
	int recv_num, reg_flag = 0;
	char from[NAME_MAX] = {}, to[NAME_MAX] = {}, buf[BUFF_MAX] = {};
	while (1) {
		recv_num = recv(sock_fd, recv_buf, BUFF_SIZE, 0);
		if (recv_num == 0) {
			if (reg_flag)
				hash_rm_user_by_name(hash, from);
			close(sock_fd);
			break;
		} else {
			switch (buf[2]) {
			case SOCK_REG:
				memcpy(from, &buf[3], NAME_MAX);
				hash_add_user(data->hash, m_name, sock_fd);
				reg_flag = 1;
				break;
			case SOCK_SND:
				memcpy(to, &buf[3], NAME_MAX);
				memcpy(&buf[3], from, NAME_MAX);
				recv_fd = hash_get_fd_by_name(hash, to);
				if (recv_fd <= 0) {
					buf[2] = SND_OFFLINE;
					send(sock_fd, recv_buf, BUFF_SIZE, 0);
				}
				if (send(recv_fd, buf, recv_num, 0) < 0)
					return; 
				break;
			default:
				return;
				break;
			}
		}
	}
	return;
}
