#include <stdio.h>
#include <string.h>
#include <errno.h>

//#define CONFIG_MULTI_USER

#include "user.h"
#ifdef CONFIG_MULTI_USER
#include "db_user.h"
#endif


struct user {
	char name[MAX_USER_NAME_LENGTH + 1];
	char password[MAX_USER_PASSWORD_LENGTH + 1];
	void *runtime;
};

#ifndef CONFIG_MULTI_USER
#define DEFAULT_USER_NAME        "default"
#define DEFAULT_USER_PASSWORD    "c21f969b5f03d33d43e04f8f136e7682" /* default */
static struct user default_user;
#endif

static struct user *user_new(char *name, char *password)
{
	struct user *user;
#ifdef CONFIG_MULTI_USER
	user = malloc(sizeof(*user));
#else
	user = &default_user;
#endif
	if (user) {
		memset(user, 0, sizeof(*user));
		strncpy(user->name, name, MAX_USER_NAME_LENGTH);
		strncpy(user->password, password, MAX_USER_PASSWORD_LENGTH);
	}
	return user;
}

int user_init(void)
{
#ifdef CONFIG_MULTI_USER
	return db_init();
#else
	(void) user_new(DEFAULT_USER_NAME, DEFAULT_USER_PASSWORD);
	return 0;
#endif
}

int user_add(char *name, char *password)
{
#ifdef CONFIG_MULTI_USER
	struct user *user;

	if (!name || !password)
		return -EINVAL;

	if (strlen(name) > MAX_USER_NAME_LENGTH || strlen(password) > MAX_USER_PASSWORD_LENGTH)
		return -EINVAL;

	user = user_new(name, password);
	if (!user)
		return -ENOMEM;

	return db_add_user(user);
#else
	fprintf(stderr, "cannot add new user in single user mode\n");
	return -EPERM;
#endif
}

struct user *user_find(char *name, char *password)
{
#ifdef CONFIG_MULTI_USER
	return db_find_user(name, password);
#else
	return &default_user;
#endif
}

int user_delete(struct user *user)
{
#ifdef CONFIG_MULTI_USER
	int ret;

	if (!user)
		return 0;

	ret = db_delete_user(user);
	if (!ret)
		free(user);

	return ret;
#else
	fprintf(stderr, "cannot delete user in single user mode\n");
	return -EPERM;
#endif
}

int user_set_runtime(struct user *user, void *runtime)
{
	if (!user)
		return -EINVAL;
	user->runtime = runtime;
	return 0;
}

void *user_get_runtime(struct user *user)
{
	return user ? user->runtime : NULL;
}

#ifdef CONFIG_USER_MODULE_TEST
int main(int argc, char *argv[])
{
	struct user *user;
	user_init();
	user=user_find("x", "y");
	printf("name %s, password %s\n", user->name, user->password);
	return 0;
}
#endif
