#ifndef __LOCKS_H__
#define __LOCKS_H__

#include "queue.h"

enum lock_types;

/**
 * Have a look at https://linux.die.net/man/3/list_head for using list_head
 */

struct thread {
	pthread_t pthread;
	unsigned long flags;
	TAILQ_ENTRY(thread) next;
};

/*************************************************
 * Spinlock
 */
struct spinlock {
	/* Fill this in */
	int lock_num;
	
};
void init_spinlock(struct spinlock *);
void acquire_spinlock(struct spinlock *);
void release_spinlock(struct spinlock *);

TAILQ_HEAD(threads, thread);
/*************************************************
 * Mutex
 */
struct mutex {
	/* Fill this in */
	struct threads waiters;
	struct spinlock spin;
	int av;
};
void init_mutex(struct mutex *);
void acquire_mutex(struct mutex *);
void release_mutex(struct mutex *);


/*************************************************
 * Semaphore
 */
struct semaphore {
	/* Fill this in */

	struct threads waiters;
	struct spinlock spin;
	int value;
	
	
};
void init_semaphore(struct semaphore *, const int);
void wait_semaphore(struct semaphore *);
void signal_semaphore(struct semaphore *);

/*************************************************
 * Lock tester.
 * Will be invoked if the program is run with -T
 */
void test_lock(void);
#endif
