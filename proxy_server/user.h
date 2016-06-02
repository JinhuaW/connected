#ifndef __USER_H__
#define __USER_H__

#define MAX_USER_NAME_LENGTH        64
#define MAX_USER_PASSWORD_LENGTH    64	/* MUST be md5 strings */

struct user;

int user_init(void);
int user_add(char *name, char *password);
int user_delete(struct user *user);
struct user *user_find(char *name, char *password);
int user_set_runtime(struct user *user, void *runtime);
void *user_get_runtime(struct user *user);

#endif
