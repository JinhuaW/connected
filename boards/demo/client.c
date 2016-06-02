#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>

#include "server.h"

int main(int argc, char *argv[])
{
		int sockfd, recv_len;
		char buff[BUFF_SIZE];
		char recv_buff[BUFF_SIZE];
		struct sockaddr_in server_addr;
		time_t cur_time;
		char *my_name = argv[1];
		char *to_name = NULL;
		int is_send = 0;
		if (strcmp(argv[2], "send") == 0) {
				is_send = 1;
				to_name = argv[3];
				printf("Send message to %s\n", to_name);
		} else {
			printf("Acting Recv message\n");
		}

		if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
				printf("create socket fialed\n");
				return 1;
		}

		memset(&server_addr, 0, sizeof(server_addr));
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(SERVICE_PORT);
		inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
		if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
				printf("Connect to server failed!\n");
				return 1;
		}

		buff[0] = 0xAA;
		buff[1] = 0x55;
		buff[2] = SOCK_REG;
		memcpy(&buff[3], my_name, NAME_MAX);

		printf("name = %s\n", my_name);
		if (send(sockfd, buff, BUFF_SIZE, 0) < 0) {
				printf("send msg error\n");
				return 1;
		}

		while (1) {
				if (is_send) { 
						buff[2] = SOCK_SND;
						memcpy(&buff[3], to_name, NAME_MAX);
						fgets(&buff[RECV_MIN], BUFF_SIZE - RECV_MIN, stdin);
						printf("sending to %s: %s\n", to_name, &buff[RECV_MIN]);
						if (send(sockfd, buff, BUFF_SIZE, 0) < 0) {
								printf("send msg error\n");
								return 1;
						}
				} else {
						if (recv(sockfd, buff, BUFF_SIZE, 0) <= 0) {
								printf("recv error, %s\n", strerror(errno));
								return 1;
						}
						printf("Recv: %s\n", &buff[RECV_MIN]);
				}
				usleep(10);
		}
		printf("client exit!\n");
		close(sockfd);
		return 0;
}	

