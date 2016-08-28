#ifndef __RB_TREE_H__
#define __RB_TREE_H__
typedef struct rb_root RB_ROOT;
typedef int (*compare_t)(void *key_data1, void *key_data2);
RB_ROOT *rb_create(compare_t compare);
int rb_insert(RB_ROOT *T, void *key_data);
#endif
