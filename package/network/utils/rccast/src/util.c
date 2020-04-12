/*
 */

#include <stdarg.h>

#include "defs.h"
#include "util.h"
#include "util_common.h"
#include "log_util.h"

/*----------------------------------------------------------------------------*/
/* globals */
/*----------------------------------------------------------------------------*/

extern log_level	slimproto_loglevel;
extern log_level	slimmain_loglevel;
extern log_level	stream_loglevel;
extern log_level	decode_loglevel;
extern log_level	output_loglevel;
extern log_level	main_loglevel;
extern log_level	util_loglevel;
extern log_level	cast_loglevel;

/*----------------------------------------------------------------------------*/
/* locals */
/*----------------------------------------------------------------------------*/
extern log_level 	util_loglevel;
static log_level 	*loglevel = &util_loglevel;



/*----------------------------------------------------------------------------*/
int pthread_cond_reltimedwait(pthread_cond_t *cond, pthread_mutex_t *mutex, u32_t msWait)
{
	struct timespec ts;
	u32_t	nsec;

	clock_gettime(CLOCK_REALTIME, &ts);

	if (!msWait) return pthread_cond_wait(cond, mutex);

	nsec = ts.tv_nsec + (msWait % 1000) * 1000000;
	ts.tv_sec += msWait / 1000 + (nsec / 1000000000);
	ts.tv_nsec = nsec % 1000000000;

	return pthread_cond_timedwait(cond, mutex, &ts);
}

/*----------------------------------------------------------------------------*/
/* 																			  */
/* QUEUE management															  */
/* 																			  */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void QueueInit(tQueue *queue, bool mutex, void (*cleanup)(void*))
{
	queue->cleanup = cleanup;
	queue->list.item = NULL;
	if (mutex) {
		queue->mutex = malloc(sizeof(pthread_mutex_t));
		pthread_mutex_init(queue->mutex, NULL);
	}
	else queue->mutex = NULL;
}

/*----------------------------------------------------------------------------*/
void QueueInsert(tQueue *queue, void *item)
{
	struct sQueue_e *list;

	if (queue->mutex) pthread_mutex_lock(queue->mutex);
	list = &queue->list;

	while (list->item) list = list->next;
	list->item = item;
	list->next = malloc(sizeof(struct sQueue_e));
	list->next->item = NULL;

	if (queue->mutex) pthread_mutex_unlock(queue->mutex);
}


/*----------------------------------------------------------------------------*/
void *QueueExtract(tQueue *queue)
{
	void *item;
	struct sQueue_e *list;

	if (queue->mutex) pthread_mutex_lock(queue->mutex);

	list = &queue->list;
	item = list->item;

	if (item) {
		struct sQueue_e *next = list->next;
		if (next->item) {
			list->item = next->item;
			list->next = next->next;
		} else list->item = NULL;
		NFREE(next);
	}

	if (queue->mutex) pthread_mutex_unlock(queue->mutex);

	return item;
}


/*----------------------------------------------------------------------------*/
void QueueFlush(tQueue *queue)
{
	struct sQueue_e *list;

	if (queue->mutex) pthread_mutex_lock(queue->mutex);

	list = &queue->list;

	while (list->item) {
		struct sQueue_e *next = list->next;
		if (queue->cleanup)	(*(queue->cleanup))(list->item);
		list = list->next;
		NFREE(next);
	}

	if (queue->mutex) {
		pthread_mutex_unlock(queue->mutex);
		pthread_mutex_destroy(queue->mutex);
		free(queue->mutex);
	}
}


/*----------------------------------------------------------------------------*/
/* 																			  */
/* LIST management															  */
/* 																			  */
/*----------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
list_t *push_item(list_t *item, list_t **list) {
  if (*list) item->next = *list;
  else item->next = NULL;

  *list = item;

  return item;
}


/*---------------------------------------------------------------------------*/
list_t *add_tail_item(list_t *item, list_t **list) {
  if (*list) {
	struct list_s *p = *list;
	while (p->next) p = p->next;
	item->next = p->next;
	p->next = item;
  } else {
	item->next = NULL;
	*list = item;
  }

  return item;
}


/*---------------------------------------------------------------------------*/
list_t *add_ordered_item(list_t *item, list_t **list, int (*compare)(void *a, void *b)) {
  if (*list) {
	struct list_s *p = *list;
	while (p->next && compare(p->next, item) <= 0) p = p->next;
	item->next = p->next;
	p->next = item;
  } else {
	item->next = NULL;
	*list = item;
  }

  return item;
}


/*---------------------------------------------------------------------------*/
list_t *pop_item(list_t **list) {
  if (*list) {
	list_t *item = *list;
	*list = item->next;
	return item;
  } else return NULL;
}


/*---------------------------------------------------------------------------*/
list_t *remove_item(list_t *item, list_t **list) {
  if (item != *list) {
	struct list_s *p = *list;
	while (p && p->next != item) p = p->next;
	if (p) p->next = item->next;
	item->next = NULL;
  } else *list = (*list)->next;

  return item;
}


/*---------------------------------------------------------------------------*/
void clear_list(list_t **list, void (*free_func)(void *)) {
  if (!*list) return;
  while (*list) {
	struct list_s *next = (*list)->next;
	if (free_func) (*free_func)(*list);
	else free(*list);
	*list = next;
  }
  *list = NULL;
}

/*----------------------------------------------------------------------------*/
unsigned Time2Int(char *Time)
{
	char *p;
	unsigned ret;

	p = strrchr(Time, ':');

	if (!p) return 0;

	*p = '\0';
	ret = atol(p + 1);

	p = strrchr(Time, ':');
	if (p) {
		*p = '\0';
		ret += atol(p + 1)*60;
	}

	p = strrchr(Time, ':');
	if (p) {
		*p = '\0';
		ret += atol(p + 1)*3600;
	}

	return ret;
}

// clock
u32_t gettime_ms(void) {
	struct timespec ts;
	if (!clock_gettime(CLOCK_MONOTONIC, &ts)) {
		return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
	}
}


void server_addr(char *server, in_addr_t *ip_ptr, u16_t *port_ptr) {
	struct addrinfo *res = NULL;
	struct addrinfo hints;
	const char *port = NULL;

    *port_ptr=8009;	
	if (strtok(server, ":")) {
		port = strtok(NULL, ":");
		if (port) {
			*port_ptr = atoi(port);
		}
	}
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	
	getaddrinfo(server, NULL, &hints, &res);
	
	if (res && res->ai_addr) {
		*ip_ptr = ((struct sockaddr_in*)res->ai_addr)->sin_addr.s_addr;
	} 
	
	if (res) {
		freeaddrinfo(res);
	}
}




