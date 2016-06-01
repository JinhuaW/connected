#include <stdio.h>
#include <pthread.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>  
#include <netinet/in.h> 
#include <stdlib.h>
#include <stdarg.h>
#include "hash.h"
#include "server.h"


void err_printf(const char *string, ...)
{	
	va_list arg;
	char buffer[2048];
	va_start(arg, string);
	vsprintf(buffer, string, arg); 
	va_end(arg);
	printf("%s", buffer);
	syslog(LOG_ERR, "%s", buffer);
}

void *accept_thread(void *arg)
{
	struct thread_data *data = (struct thread_data *)arg;
	struct sockaddr_in host_addr;
	struct sockaddr_in client_addr;
	int listen_sock = 0;
	int sock_len = sizeof(client_addr);
	memset((void *)&host_addr, 0, sizeof(struct sockaddr_in));
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(SERVICE_PORT);
	host_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	listen_sock = socket(AF_INET, SOCK_STREAM, 0);
	if (listen_sock < 0) {
		err_printf("%s:create socked failed\n", __FILE__);
		exit(1);
	}
	if (bind(listen_sock, (struct sockaddr *)&host_addr, sizeof(host_addr)) < 0) {
		err_printf("%s:bind socket failed\n", __FILE__);
		exit(1);
	}
	if (listen(listen_sock, data->max_user) < 0) {
		err_printf("%s:listen socket failed\n", __FILE__);
		exit(1);
	}
	sleep(1); 
	while (1) {
		pthread_mutex_lock(&data->mutex);
		if (data->user_count < data->ready_count) {
			pthread_mutex_unlock(&data->mutex);
			while (data->new_user_fd)
				usleep(10);
			data->new_user_fd = accept(listen_sock, (struct sockaddr *)&client_addr, &sock_len);
			if (data->new_user_fd < 0) {
				data->new_user_fd = 0;
				err_printf("%d: accpet new socket failed, %s\n", __LINE__, strerror(errno));
				continue;
			}
			printf("new sock fd %d\n", data->new_user_fd);
			pthread_mutex_lock(&data->mutex);
			data->user_count++;
			pthread_cond_signal(&data->cond);
			pthread_mutex_unlock(&data->mutex);
			
		} else {
			pthread_mutex_unlock(&data->mutex);
			err_printf("%s: Server is now full\n", __FILE__);
			usleep(10);
		} 	
	}
	pthread_exit(NULL);
}

void *process_thread(void *arg)
{
	int sock_fd, recv_num, recv_fd, reg_flag;
	struct thread_data *data = (struct thread_data *)arg;
	char buf[BUFF_SIZE], m_name[NAME_MAX], t_name[NAME_MAX];
	char *recv_buf;

	while (1) {
		pthread_mutex_lock(&data->mutex);
		data->ready_count++;
		pthread_cond_wait(&data->cond, &data->mutex);
		data->ready_count--;
		pthread_mutex_unlock(&data->mutex);
		while (!data->new_user_fd)
			usleep(10);
		sock_fd = data->new_user_fd;
		data->new_user_fd = 0;
		reg_flag = 0;
		recv_buf = buf;
		for (;;) {
			recv_num = recv(sock_fd, recv_buf, BUFF_SIZE, 0);
			printf("%d recved %d message", sock_fd, recv_num);
			for ( int i = 0; i < 20; i++) {
				printf("0x%x ", buf[i]);
			}
			printf("\n");
			if (recv_num == 0) {
				if (reg_flag)
					hash_rm_user_by_name(data->hash, m_name);
				close(sock_fd);
				break;
			} else {
				switch (buf[2]) {
				case SOCK_REG:
					memcpy(m_name, &buf[3], NAME_MAX);
					printf("Add user (%s), fd = %d\n", m_name, sock_fd);
					hash_add_user(data->hash, m_name, sock_fd);
					reg_flag = 1;
					break;
				case SOCK_SND:
					memcpy(t_name, &buf[3], NAME_MAX);
					memcpy(&buf[3], m_name, NAME_MAX);
					recv_fd = hash_get_fd_by_name(data->hash, t_name);
					printf("try to Send to %s by fd = %d\n", t_name, recv_fd);
					if (recv_fd <= 0) {
						//return error to the client.
						send(sock_fd, recv_buf, BUFF_SIZE, 0);
					}
					printf("sending to %d: %s\n", recv_fd, &buf[RECV_MIN]);
					send(recv_fd, buf, recv_num, 0);
					break;
				default:
					break;
				}
			}
		}
		pthread_mutex_lock(&data->mutex);
		data->user_count--;
		pthread_mutex_unlock(&data->mutex);
	}
	pthread_exit(NULL);
}


int main(int argc, char *argv[])
{
	struct thread_data data;
	pthread_t server_thread, *client_thread;
	int max_user, i;
	
	if (argc !=2) {
		err_printf("Usage: %s max_user\n", argv[0]);
		return 1;
	}
	sscanf(argv[1], "%d", &max_user);
	printf("max_user = %d\n", max_user);

	memset(&data, 0, sizeof(data));	

	pthread_cond_init(&data.cond, NULL);
	pthread_mutex_init(&data.mutex, NULL);
	data.max_user = max_user;
	data.hash = hash_init(max_user);

	if (data.hash == NULL) {
		err_printf("Fail to create hash table!\n");
		return 1;
	}

	client_thread = (pthread_t *)malloc(sizeof(pthread_t) * max_user);
	if (NULL == client_thread) {
		err_printf("Fail to alloc buff for client threads!");
		return 1;
	}

	if (pthread_create(&server_thread, NULL, accept_thread,(void *)&data)) {
		err_printf("create server thread failed\n");
		return 1;
	}

	for (i = 0;i < max_user;i++) {
		if (pthread_create(&client_thread[i], NULL, process_thread, (void *)&data)) {
			err_printf("Create client thread %d failed\n", i);
			return 1;
		}
	}
	
	pthread_join(server_thread, NULL);
	free(client_thread);
	return 0;
}
