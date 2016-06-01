all:
	gcc server.c hash.c -o IPC_server -lpthread
	gcc client.c -o client
clean:
	rm -rf IPC_server
	rm -rf client
