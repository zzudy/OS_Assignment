#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include "config.h"
#include "locks.h"

static int nr_slots = 0;

static enum lock_types lock_type;
static struct spinlock spnlock;
static struct mutex mu;

static struct semaphore sema_full;
static struct semaphore sema_empty;
static struct semaphore mutex;

void (*enqueue_fn)(int value) = NULL;
int (*dequeue_fn)(void) = NULL;

typedef struct queue{

	int front;
	int rear;
	int *buffer;

}QUEUE;

static QUEUE *que_ue;



void enqueue_ringbuffer(int value)
{
	assert(enqueue_fn);
	assert(value >= MIN_VALUE && value < MAX_VALUE);

	enqueue_fn(value);
}

int dequeue_ringbuffer(void)
{
	int value;

	assert(dequeue_fn);

	value = dequeue_fn();
	assert(value >= MIN_VALUE && value < MAX_VALUE);

	return value;
}

 int isFull()
{
    if((que_ue->rear + 1) % nr_slots ==  que_ue->front)  
        return 1;
    return 0;
}

int isEmpty()
{
    if(que_ue->front == que_ue->rear) return 1;
    return 0;
}

/*********************************************************************
 * TODO: Implement using spinlock
 */
void enqueue_using_spinlock(int value)
{
LOCK:

   acquire_spinlock(&spnlock);
   
   if(isFull() ==1){

    release_spinlock(&spnlock);
   // sched_yield();
    goto LOCK;

    }else{
		que_ue->rear = (que_ue->rear+1) %nr_slots;
		que_ue->buffer[que_ue->rear] = value;

    }
   release_spinlock(&spnlock);

   return;
}

int dequeue_using_spinlock(void)
{
	int value;
   LOCK:

   acquire_spinlock(&spnlock);
    if(isEmpty() ==1){
     release_spinlock(&spnlock);
    // sched_yield();
    goto LOCK;
    }

    else{
        que_ue->front = (que_ue->front + 1) % nr_slots;
        value = que_ue->buffer[que_ue->front];
            
    }
   release_spinlock(&spnlock);
   
   return value;


}

void init_using_spinlock(void)
{
	enqueue_fn = &enqueue_using_spinlock;
	dequeue_fn = &dequeue_using_spinlock;
	init_spinlock(&spnlock);
}

void fini_using_spinlock(void)
{
	free(que_ue->buffer);
	free(que_ue);
}


/*********************************************************************
 * TODO: Implement using mutex
 */
void enqueue_using_mutex(int value)
{
	LOCK:

   acquire_mutex(&mu);
   
   if(isFull() ==1){

    release_mutex(&mu);
   
    goto LOCK;

    }else{
        que_ue->rear = (que_ue->rear+1) %nr_slots;
       que_ue->buffer[que_ue->rear] = value;

    }
   release_mutex(&mu);

   return;
}

int dequeue_using_mutex(void)
{
	int value;


   LOCK:
 
   acquire_mutex(&mu);
    if(isEmpty() ==1){
     release_mutex(&mu);
    goto LOCK;
    }

    else{
        que_ue->front = (que_ue->front + 1) % nr_slots;
        value = que_ue->buffer[que_ue->front];
            
    }
   release_mutex(&mu);
   
   return value;
}

void init_using_mutex(void)
{
	enqueue_fn = &enqueue_using_mutex;
	dequeue_fn = &dequeue_using_mutex;
	init_mutex(&mu);
}

void fini_using_mutex(void)
{
	free(que_ue->buffer);
	free(que_ue);
}


/*********************************************************************
 * TODO: Implement using semaphore
 */
void enqueue_using_semaphore(int value)
{
   LOCK:
   wait_semaphore(&sema_empty);
   wait_semaphore(&mutex);
   
   if(isFull() ==1){

   signal_semaphore(&mutex);
   signal_semaphore(&sema_full);
   
    goto LOCK;

    }else{
        que_ue->rear = (que_ue->rear+1) %nr_slots;
        que_ue->buffer[que_ue->rear] = value;

    }

   signal_semaphore(&mutex);
   signal_semaphore(&sema_full);

   return;
}

int dequeue_using_semaphore(void)
{
	int value;


   LOCK:
 
   wait_semaphore(&sema_full);
   wait_semaphore(&mutex);

    if(isEmpty() ==1){
        signal_semaphore(&mutex);
        signal_semaphore(&sema_empty);
    goto LOCK;

    }

    else{
        que_ue->front = (que_ue->front + 1) % nr_slots;
        value = que_ue->buffer[que_ue->front];
            
    }
	
   signal_semaphore(&mutex);
   signal_semaphore(&sema_empty);
   
   return value;
}

void init_using_semaphore(void)
{
	enqueue_fn = &enqueue_using_semaphore;
	dequeue_fn = &dequeue_using_semaphore;

	init_semaphore(&mutex,1);
	init_semaphore(&sema_empty,7);
	init_semaphore(&sema_full,0);

}

void fini_using_semaphore(void)
{
	free(que_ue->buffer);
	free(que_ue);
}


/*********************************************************************
 * Common implementation
 */
int init_ringbuffer(const int _nr_slots_, const enum lock_types _lock_type_)
{
	assert(_nr_slots_ > 0);
	nr_slots = _nr_slots_;
	/* Initialize lock! */
	

	switch (lock_type) {

	case lock_spinlock:
		init_using_spinlock();
		break;
	case lock_mutex:
		init_using_mutex();
		break;
	case lock_semaphore:
		init_using_semaphore();
		break;
	}
		

	/* TODO: Initialize your ringbuffer and synchronization mechanism */
	que_ue = malloc(sizeof(QUEUE));
	que_ue-> buffer= (int *)malloc(sizeof(int)* nr_slots);
	
	que_ue->front = 0;
	que_ue->rear = 0;
	lock_type = _lock_type_;
	return 0;
}

void fini_ringbuffer(void)
{
	/* TODO: Clean up what you allocated */
	switch (lock_type) {
	case lock_spinlock:
		fini_using_spinlock();
		break;
	case lock_mutex:
		fini_using_mutex();
		break;
	case lock_semaphore:
		fini_using_semaphore();
		break;
	default:
		break;
	}
}

