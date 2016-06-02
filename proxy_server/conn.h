#ifndef __CONN_H__
#define __CONN_H__

enum conn_state {
	CONN_STATE_FREE = 0,
	CONN_STATE_CONNECTED = 1,
	CONN_STATE_AUTHERTICATED = 2,
	CONN_STATE_DISCONNECTED = 3,
	CONN_STATE_NR
};

struct conn {
	int fd;
	int state;
	void *private_data;
};

#define conn_valid_state(state) (((state)>0) && ((state)<CONN_STATE_NR))

struct conn *conn_new(int fd, void *data);
void conn_delete(struct conn *conn);
int conn_set_state(struct conn *conn, int state);
int conn_get_state(struct conn *conn);

#endif
