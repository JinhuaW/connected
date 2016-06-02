#include "conn.h"

struct conn *conn_new(int fd, void *data)
{
	struct conn *conn = malloc(sizeof(*conn));
	if (conn) {
		conn->fd = fd;
		conn->state = CONN_STATE_FREE;
		conn->private_datae = data;
	}
	return conn;
}

void conn_delete(struct conn *conn)
{
	if (conn)
		free(conn);
}

int conn_set_state(struct conn *conn, int state)
{
	//BUG_ON(conn == NULL);
	if (!conn_valid_state(state))
		return -EINVAL;
	conn->state = state;
	return 0;
}

int conn_get_state(struct conn *conn)
{
	//BUG_ON(conn == NULL);
	return conn->state;
}
