/*
 */

#ifndef __UTIL_H
#define __UTIL_H

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "pthread.h"
#include "util_common.h"
//#include "jansson.h"

typedef struct {
	pthread_mutex_t	*mutex;
	void (*cleanup)(void*);
	struct sQueue_e {
		struct sQueue_e *next;
		void 			*item;
	} list;
} tQueue;

typedef struct list_s {
	struct list_s *next;
} list_t;

list_t*		push_item(list_t *item, list_t **list);
list_t*		add_tail_item(list_t *item, list_t **list);
list_t*		add_ordered_item(list_t *item, list_t **list, int (*compare)(void *a, void *b));
list_t*		pop_item(list_t **list);
list_t*   	remove_item(list_t *item, list_t **list);
void 		clear_list(list_t **list, void (*free_func)(void *));

void 		QueueInit(tQueue *queue, bool mutex, void (*f)(void*));
void 		QueueInsert(tQueue *queue, void *item);
void 		*QueueExtract(tQueue *queue);
void 		QueueFlush(tQueue *queue);

unsigned 	Time2Int(char *Time);
int			pthread_cond_reltimedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, u32_t msWait);
void 		server_addr(char *server, in_addr_t *ip_ptr, u16_t *port_ptr);


#endif
