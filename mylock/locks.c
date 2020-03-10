#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include "config.h"
#include "locks.h"
#include "atomic.h"


/******************************************************************
 * Spinlock implementation
 */
void init_spinlock(struct spinlock *lock)
{
	lock->lock_num =0;
	return;
}

void acquire_spinlock(struct spinlock *lock)
{
	while(compare_and_swap(&(lock->lock_num),0,1) != 0);
	return;
}

void release_spinlock(struct spinlock *lock)
{
	compare_and_swap(&(lock->lock_num),1,0);
	return;
}


/******************************************************************
 * Blocking lock implementation
 *
 * Hint: Use pthread_self, pthread_kill, pause, and signal
 */
void init_mutex(struct mutex *lock)
{
	TAILQ_INIT(&lock->waiters);
	lock->av=0;
	return;
}
void handlr(int signum)
{
    if(signum == SIGUSR1){
		return;
    }

}

void acquire_mutex(struct mutex *lock)
{
	acquire_spinlock(&lock->spin);

	if(lock->av){

		struct thread *t;
		t = malloc(sizeof(*t));
		
		t->pthread = pthread_self();
		t->flags = 0;

		 TAILQ_INSERT_TAIL(&lock->waiters,t,next);

		signal(SIGUSR1, handlr);
		release_spinlock(&lock->spin);

		pause();


		t->flags =1;

	}else{
		compare_and_swap(&lock->av,0,1);

		release_spinlock(&lock->spin);
		
	}
	return;
}

void release_mutex(struct mutex *lock)
{
	acquire_spinlock(&lock->spin);

	if(!TAILQ_EMPTY(&lock->waiters)){ 

	struct thread *t= (struct thread *)TAILQ_FIRST(&lock->waiters);

	
	TAILQ_REMOVE(&lock->waiters,t,next);

again :

	pthread_kill(t->pthread, SIGUSR1);

	if(t->flags == 0){
   		usleep(100);

		goto again;

	}

	free(t);
	release_spinlock(&lock->spin);

	}
	else {
		lock->av=0;

		release_spinlock(&lock->spin);
	}
	return;
}





/******************************************************************
 * Semaphore implementation
 *
 * Hint: Use pthread_self, pthread_kill, pause, and signal
 */
void init_semaphore(struct semaphore *sem, int S)
{
	 sem->value = S;
	 TAILQ_INIT(&sem->waiters);


	return;
}


void wait_semaphore(struct semaphore *sem)
{
	acquire_spinlock(&sem->spin);

	sem->value--;

	if(sem->value<=0){

		struct thread *t;
		t = malloc(sizeof(*t));
		
		t->pthread = pthread_self();
		t->flags = 0;

		TAILQ_INSERT_TAIL(&sem->waiters, t, next);
		signal(SIGUSR1, handlr);
		release_spinlock(&sem->spin);

		pause();

		t->flags =1;

		
	}
	else{

		release_spinlock(&sem->spin);
	}

	return;
}

void signal_semaphore(struct semaphore *sem)
{

	acquire_spinlock(&sem->spin);

	sem->value++;

	if(sem->value<=0){ 
	struct thread *t= (struct thread *)TAILQ_FIRST(&(sem->waiters));

	
	TAILQ_REMOVE(&(sem->waiters),t,next);

again :

	pthread_kill(t->pthread, SIGUSR1);

	if(t->flags == 0){
   		usleep(100);

		goto again;

	}

	free(t);
	release_spinlock(&(sem->spin));

	}
	else{
		release_spinlock(&sem->spin);
	}
	return;
}


/******************************************************************
 * Spinlock tester exmaple
 */
struct spinlock testlock;
int testlock_held = 0;

void *test_thread(void *_arg_)
{
	usleep(random() % 1000 * 1000);

	printf("Tester acquiring the lock...\n");
	acquire_spinlock(&testlock);
	printf("Tester acquired\n");
	assert(testlock_held == 0);
	testlock_held = 1;

	sleep(1);

	printf("Tester releases the lock\n");
	testlock_held = 0;
	release_spinlock(&testlock);
	printf("Tester released the lock\n");
	return 0;
}

void test_lock(void)
{
	/* Set nr_testers as you need
	 *  1: one main, one tester. easy :-)
	 * 16: one main, 16 testers contending the lock :-$
	 */
	const int nr_testers = 1;
	int i;
	pthread_t tester[nr_testers];

	printf("Main initializes the lock\n");
	init_spinlock(&testlock);

	printf("Main graps the lock...");
	acquire_spinlock(&testlock);
	assert(testlock_held == 0);
	testlock_held = 1;
	printf("acquired!\n");

	for (i = 0; i < nr_testers; i++) {
		pthread_create(tester + i, NULL, test_thread, NULL);
	}

	sleep(1);

	printf("Main releases the lock\n");
	testlock_held = 0;
	release_spinlock(&testlock);
	printf("Main released the lock\n");

	for (i = 0; i < nr_testers; i++) {
		pthread_join(tester[i], NULL);
	}
	assert(testlock_held == 0);
	printf("Your spinlock implementation looks O.K.\n");

	return;
}

