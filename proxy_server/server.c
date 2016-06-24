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
#include <hash.h>
#include <conu.h>
#include "server.h"
#include <log.h>
#include <time_info.h>

void *accept_thread(void *arg)
{
	struct thread_data *data = (struct thread_data *)arg;
	struct sockaddr_in client_addr;
	int listen_sock = 0;
	int sock_len = sizeof(client_addr);
	char cur_time[BUFF_MAX];

	listen_sock = new_server(SERVICE_PORT, data->max_user);
	if (listen_sock < 0) {
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
				log_printf(LOG_ERR, "[%s]Accpet new socket failed, %s\n", get_cur_time(cur_time), strerror(errno));
				continue;
			}
			log_printf(LOG_DEBUG, "[%s]Create sock fd %d\n", get_cur_time(cur_time), data->new_user_fd);
			pthread_mutex_lock(&data->mutex);
			data->user_count++;
			pthread_cond_signal(&data->cond);
			pthread_mutex_unlock(&data->mutex);
			
		} else {
			pthread_mutex_unlock(&data->mutex);
			log_printf(LOG_WARNING, "[%s]Server is now full\n", get_cur_time(cur_time));
			usleep(10);
		} 	
	}
	pthread_exit(NULL);
}

void *process_thread(void *arg)
{
	int sock_fd;
	struct thread_data *data = (struct thread_data *)arg;

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
		conu_process(sock_fd, data->hash);
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
		log_printf(LOG_CRIT, "Usage: %s max_user\n", argv[0]);
		return 1;
	}
	sscanf(argv[1], "%d", &max_user);

	log_printf(LOG_DEBUG, "max_user = %d\n", max_user);

	memset(&data, 0, sizeof(data));	

	pthread_cond_init(&data.cond, NULL);
	pthread_mutex_init(&data.mutex, NULL);
	data.max_user = max_user;
	data.hash = hash_init(max_user);

	if (data.hash == NULL) {
		log_printf(LOG_CRIT, "Fail to create hash table!\n");
		return 1;
	}

	client_thread = (pthread_t *)malloc(sizeof(pthread_t) * max_user);
	if (NULL == client_thread) {
		log_printf(LOG_CRIT, "Fail to alloc buff for client threads!");
		return 1;
	}

	if (pthread_create(&server_thread, NULL, accept_thread,(void *)&data)) {
		log_printf(LOG_CRIT, "create server thread failed\n");
		return 1;
	}

	for (i = 0;i < max_user;i++) {
		if (pthread_create(&client_thread[i], NULL, process_thread, (void *)&data)) {
			log_printf(LOG_CRIT, "Create client thread %d failed\n", i);
			return 1;
		}
	}
	
	pthread_join(server_thread, NULL);
	free(client_thread);
	return 0;
}
