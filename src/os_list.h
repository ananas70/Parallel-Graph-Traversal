/* SPDX-License-Identifier: BSD-3-Clause */

/*
 * Heavily inspired for Linux kernel code:
 * https://github.com/torvalds/linux/blob/master/include/linux/list.h
 */

#ifndef __OS_LIST_H__
#define __OS_LIST_H__	1

#include <stddef.h>

typedef struct os_list_node_t {
	struct os_list_node_t *prev, *next;
} os_list_node_t;

static inline void list_init(os_list_node_t *head)
{
	head->prev = head;
	head->next = head;
}

static inline void list_add(os_list_node_t *head, os_list_node_t *node)
{	//la dreapta lui head (dupa head)
	node->next = head->next;
	node->prev = head;
	head->next->prev = node;
	head->next = node;
}

static inline void list_add_tail(os_list_node_t *head, os_list_node_t *node)
{	//la stanga lui head (inaintea lui head)
	node->prev = head->prev;
	node->next = head;
	head->prev->next = node;
	head->prev = node;
}

static inline void list_del(os_list_node_t *node)
{
	node->prev->next = node->next;
	node->next->prev = node->prev;
	node->next = node;
	node->prev = node;
}

static inline int list_empty(os_list_node_t *head)
{
	return (head->next == head);
}

/*Returns a pointer to the structure that contains the os_list_node_t.
 It does this by subtracting the offset of the os_list_node_t within the structure
from the given pointer.
	Example usage:
os_list_node_t *node;
os_task_t *task = list_entry(node, os_task_t, list);*/

#define list_entry(ptr, type, member) ({			\
		void *tmp = (void *)(ptr);			\
		(type *) (tmp - offsetof(type, member));	\
	})


/*Iterates over a linked list
	Example usage:
os_list_node_t *pos;
list_for_each(pos, &some_list) {
    // Do something with 'pos'
}
*/
#define list_for_each(pos, head) \
		for (pos = (head)->next; pos != (head); pos = pos->next)


/*A variation of list_for_each that includes a temporary variable tmp.
This is useful when deleting elements during iteration since it ensures
that the iterator (pos) is not affected by the removal of elements.
	Example:
os_list_node_t *pos, *tmp;
list_for_each_safe(pos, tmp, &some_list) {
    // Do something with 'pos'
    // It's safe to remove or free 'pos' during this loop
}
*/
#define list_for_each_safe(pos, tmp, head) \
	for (pos = (head)->next, tmp = pos->next; pos != (head); \
			pos = tmp, tmp = pos->next)

#endif
