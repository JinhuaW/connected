#include <stdlib.h>
#include "rb_tree.h"
#include <stdio.h>
typedef enum rb_color {
	BLACK = 0,
	RED = 1
} CLR;

typedef struct rb_node {
	struct rb_node *parent;
	struct rb_node *left;
	struct rb_node *right;
	void *key_data;
	CLR color;
} RB_NODE;

typedef struct rb_root {
	RB_NODE *root;
	RB_NODE *nil;
	compare_t compare;
	show_t show;
} RB_ROOT;


static void rb_left_rotate(RB_ROOT *T, RB_NODE *node) 
{
	if (node->right != T->nil) {
		RB_NODE *y = node->right;
		node->right = y->left;
		if (y->left != T->nil)
			y->left->parent = node;
		y->parent = node->parent;
		if (node->parent == T->nil) {
			T->root = y;
		} else {
			if (node == node->parent->left)
				node->parent->left = y;
			else
				node->parent->right = y;
		}
		y->left = node;
		node->parent = y;
	}
}

static void rb_right_rotate(RB_ROOT *T, RB_NODE *node)
{
	if (node->left != T->nil) {
		RB_NODE *y = node->left;
		node->left = y->right;
		if (y->right != T->nil)
			y->right->parent = node;
		
		y->parent = node->parent;
		if (node->parent == T->nil) {
			T->root = y;
		} else {
			if (node == node->parent->left)
				node->parent->left = y;
			else
				node->parent->right = y;
		}
		node->parent = y;
		y->right = node;
		
	}
}

static void rb_insert_fixup(RB_ROOT *T, RB_NODE *z)
{
	RB_NODE *y;
	while (z->parent->color == RED) {
		if (z->parent == z->parent->parent->left) {
			y = z->parent->parent->right;
			if (y->color == RED) {
				y->color = BLACK;
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			} else {
				if (z == z->parent->right) {
					z = z->parent;
					rb_left_rotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rb_right_rotate(T, z->parent->parent);
			}
		} else {
			y = z->parent->parent->left;
			if (y->color == RED) {
				y->color = BLACK;
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				z = z->parent->parent;
			} else {
				if (z == z->parent->left) {
					z = z->parent;
					rb_right_rotate(T, z);
				}
				z->parent->color = BLACK;
				z->parent->parent->color = RED;
				rb_left_rotate(T, z->parent->parent);
			}
		}
	}
	T->root->color = BLACK;
}

RB_ROOT *rb_create(compare_t compare, show_t show)
{
	RB_ROOT *T = NULL;
	if (compare == NULL)
		return NULL;
	T = (RB_ROOT *)calloc(1, sizeof(RB_ROOT));
	if (T == NULL)
		return NULL;
	T->compare = compare;
	T->show = show;
	return T;
}

int rb_insert(RB_ROOT *T, void *key_data)
{
	if (T == NULL || key_data == NULL)
		return 1;
	if (T->root == NULL) {
		T->root = (RB_NODE *)calloc(1, sizeof(RB_NODE));
		T->nil = (RB_NODE *)calloc(1, sizeof(RB_NODE));
		if (T->root == NULL || T->nil == NULL)
			return 1;
		T->nil->color = BLACK;
		T->root->parent = T->nil;
		T->root->left = T->nil;
		T->root->right = T->nil;
		T->root->key_data = key_data;
		T->root->color = BLACK;
	} else {
		RB_NODE *x = T->root;
		RB_NODE *p = T->nil;
		while (x != T->nil) {
			p = x;
			switch (T->compare(key_data, x->key_data)) {
			case 1:
				x = x->right;
				break;
			case -1:
				x = x->left;
				break;
			case 0:
				return 0;
			default:
				printf("unknown compare result\n");
				return 1;
			}
		}
		x = (RB_NODE *)calloc(1, sizeof(RB_NODE));
		if (x == NULL)
			return 1;
		x->parent = p;
		x->left = T->nil;
		x->right = T->nil;
		x->key_data = key_data;
		x->color = RED;
		if (T->compare(key_data, p->key_data) == 1)
			p->right = x;
		else
			p->left = x;
		rb_insert_fixup(T, x);
	}
	return 0;
}


static __rb_show(RB_ROOT *T, RB_NODE *x, int level)
{
	if (x == T->nil)
		return;
	printf("curent: ");
	T->show(level, x->key_data);
	if (x->parent != T->nil) {
		printf("parent: ");
		T->show(level-1, x->parent->key_data);
	}
	if (x->left != T->nil) {
		printf("left: ");
		T->show(level+1, x->left->key_data);
	}
	if (x->right != T->nil) {
		printf("right: ");
		T->show(level+1, x->right->key_data);
	}
	printf("======================================\r\n");
	__rb_show(T, x->left, level+1);
	__rb_show(T, x->right, level+1);

}

void rb_show(RB_ROOT *T)
{
	if (T->show == NULL)
		return;
	__rb_show(T, T->root, 1);	
}

void *rb_find(RB_ROOT *T, void *key_data)
{
	int ret;
	if (T == NULL || key_data == NULL)
		return NULL;
	RB_NODE *p = T->root;
	while (p != T->nil) {
		ret = T->compare(key_data, p->key_data);
		if (ret == 0)
			return p->key_data;
		else if (ret == 1)
			p = p->right;
		else
			p = p->left;
	}
	return NULL;
}
