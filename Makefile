all:
	gcc server.c hash.c -o IPC_server -lpthread
clean:
	rm -rf IPC_server
	rm -rf client
