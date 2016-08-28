#ifndef __RB_TREE_H__
#define __RB_TREE_H__
typedef struct rb_root RB_ROOT;
typedef int (*compare_t)(void *key_data1, void *key_data2);
typedef void (*show_t)(int level, void *key_data);
RB_ROOT *rb_create(compare_t compare, show_t show);
int rb_insert(RB_ROOT *T, void *key_data);
void *rb_find(RB_ROOT *T, void *key_data);
void rb_show(RB_ROOT *T);
#endif
