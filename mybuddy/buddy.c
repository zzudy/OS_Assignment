/**********************************************************************
 * Copyright (c) 2018
 *	Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/
#include <stdio.h>
#include <errno.h>
#include <malloc.h>
#include "config.h"

#include "queue.h"

/**
 * Skeleton data structures to implement the buddy system allocator
 */

/**
 * Data structure to represent an order-@order pages. To the rest of this file,
 * consecutive pages will be represented in @start:@order notation.
 * E.g., 16:3 is 8(2^3)  consecutive pages (or say order-3 page) starting from
 * page frame 16.
 */


struct chunk {
	/**
	 * TODO: Modify this structure as you need.
	 */
	unsigned int start; // address
	unsigned int order;

	TAILQ_ENTRY(chunk) next;
};


/**
 * Data structure to maintain order-@order free chunks.
 * NOTE that chunk_list SHOULD WORK LIKE THE QUEUE; the firstly added chunk
 * should be used first, otherwise the grading system will fail.
 */
struct chunk_list {
	/**
	 * TODO: Modify this structure as you need
	 */
	TAILQ_HEAD(chunkss, chunk) chu;
	unsigned int order;
};


/**
 * Data structure to realize the buddy system allocator
 */

struct buddy {
	/**
	 * TODO: Modify this example data structure as you need
	 */

	/**
	 * Free chunk list in the buddy system allocator.
	 *
	 * @NR_ORDERS is @MAX_ORDER + 1 (considering order-0 pages) and deifned in
	 * config.h. @MAX_ORDER is set in the Makefile. MAKE SURE your buddy
	 * implementation can handle order-0 to order-@MAX_ORDER pages.
	 */
	struct chunk_list chunks[NR_ORDERS];
	unsigned int allocated;	/* Number of pages that are allocated */
	unsigned int free;		/* Number of pages that are free */
};


/**
 * This is your buddy system allocator instance!
 */
static struct buddy buddy;


/**
 *    Your buddy system allocator should manage from order-0 to
 *  order-@MAX_ORDER. In the following example, assume your buddy system
 *  manages page 0 to 0x1F (0 -- 31, thus @nr_pages is 32) and pages from
 *  20 to 23 and 28 (0x14 - 0x17, 0x1C) are allocated by alloc_pages()
 *  by some orders.
 *  At this moment, the buddy system will split the address space into;
 *
 *      0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F
 * 0x00 <-------------------4-------------------------->
 * 0x10 <----2----->X  X  X  X  <-----2---->X  <0><-1-->
 *                  ^  ^  ^  ^              ^
 *                  |--|--|--|--------------|
 *                     allocated
 *
 *   Thus, the buddy system will maintain the free chunk lists like;
 *
 * Order     | Start addresses of free chunks
 * ----------------------------------------------
 * MAX_ORDER |
 *    ...    |
 *     4     | 0x00
 *     3     |
 *     2     | 0x10, 0x18
 *     1     | 0x1e
 *     0     | 0x1d
 */


/**
 * Allocate 2^@order contiguous pages.
 *
 * Description:
 *    For example, when @order=0, allocate a single page, @order=2 means
 *  to allocate 4 consecutive pages, and so forth.
 *    From the example state above, alloc_pages(2) gives 0x10 through @*page
 *  and the corresponding entry is removed from the free chunk. NOTE THAT the
 *  free chunk lists should be maintained as 'FIFO' so alloc_pages(2) returns 
 *  0x10, not 0x18. 
 *    To hanle alloc_pages(3), the order-4 chunk (0x00 -- 0x0f) should
 *  be broken into smaller chunks (say 0x00 -- 0x07, 0x08 -- 0x0f), and
 *  the LEFT BUDDY will be returned through @page whereas RIGHT BUDDY
 *  will be put into the order-3 free chunk list.
 *
 * Return:
 *   0      : On successful allocation. @*page will contain the starting
 *            page number of the allocated chunk
 *  -EINVAL : When @order < 0 or @order > MAX_ORDER
 *  -ENOMEM : When order-@order contiguous chunk is not available in the system
 */
int alloc_pages(unsigned int *page, const unsigned int order)
{
	buddy.allocated += (1 << order);
	buddy.free -= (1 << order);

	int i=0;
	unsigned int add=0;
	struct chunk *ch;
	if(order<0 || order>MAX_ORDER){
		return -EINVAL;
	}
	int count=0;
	for(int a=order; a<MAX_ORDER+1; a++){
		if(TAILQ_FIRST(&(buddy.chunks[a].chu)) != NULL){
			count++;
		}
	}
	if(count == 0){
		return -ENOMEM;
	}
	//TAILQ_EMPTY(&(buddy.chunks[order].chu))
	//if(TAILQ_FIRST(&(buddy.chunks[order].chu)) != NULL){
		if(!TAILQ_EMPTY(&(buddy.chunks[order].chu))){
		struct chunk *c= TAILQ_FIRST(&(buddy.chunks[order].chu));
		*page = c->start;
		//fprintf(stderr, "Dddddddd\n");
		TAILQ_REMOVE(&(buddy.chunks[order].chu),c,next);
		free(c);
		//c = NULL;
		return 0;
	}
	else{
		for(i=order+1; i<MAX_ORDER+1; i++){ // find j
			if(TAILQ_FIRST(&(buddy.chunks[i].chu)) != NULL){
				break;
			}
		}
		if(i==MAX_ORDER && TAILQ_FIRST(&(buddy.chunks[i].chu)) == NULL){
			//fprintf(stderr, "Dddddddd\n");
			return -ENOMEM;
		}
		ch = TAILQ_FIRST(&(buddy.chunks[i].chu));
		TAILQ_REMOVE(&(buddy.chunks[i].chu),ch,next);
		for(int j=i; j>order; j--){
			add = ch->start+  (1 << (j-1));

			struct chunk *se = malloc(sizeof(struct chunk));
			se->start = add; 
			se->order = j-1;
			TAILQ_INSERT_TAIL(&(buddy.chunks[j-1].chu),se,next);
		}
		*page = ch->start;
		free(ch);
		
	}
	
	return 0;
}

struct chunk* find_buddy(unsigned int page, int order){
	
	struct chunk *temp;
	unsigned int l = page;
	//se->start = page;
	if(page % (1<<(order+1)) == 0){ // left buddy
		l += (1<<order);
	}else{
		l -= (1<<order);
	}
	for(temp = buddy.chunks[order].chu.tqh_first; temp; ){
		if(temp->start == l){ // buddy ;
		//fprintf(stderr,"FIMD %x %x %d\n",page, temp->start,order);
			return temp; //exist
		}
		//if(temp == *buddy.chunks[order].chu.tqh_last) break;
		temp = temp->next.tqe_next;
	}
	//fprintf(stderr,"__ page = %x l =%x_ order %d_\n", page,l,(1<<order));
	return NULL; // nobuddy
}
void free_pages(unsigned int page, const unsigned int order)
{
	buddy.allocated -= (1 << order);
	buddy.free += (1 << order);

	struct chunk *se;
	struct chunk *sa;
	struct chunk *temp;
	int a=order;

	if(TAILQ_FIRST(&(buddy.chunks[order].chu)) == NULL){
		//fprintf(stderr,"NULL\n");
		se = (struct chunk*)malloc(sizeof(struct chunk));
		se->start = page;
		se->order= order;
		TAILQ_INSERT_TAIL(&(buddy.chunks[order].chu),se,next);
		return;
	}
	se = (struct chunk*)malloc(sizeof(struct chunk));
	se->start = page;
	se->order= a;
	TAILQ_INSERT_TAIL(&(buddy.chunks[a].chu),se,next);
	
	while(1){
		if(a>=MAX_ORDER) {
			break;
		}
		temp = find_buddy(page,a);
		
		if(temp!=NULL){
			se = find_buddy(temp->start, a);
			TAILQ_REMOVE(&(buddy.chunks[a].chu),temp, next);
			free(temp);
			temp = NULL;
			if(se != NULL){
				TAILQ_REMOVE(&(buddy.chunks[a].chu),se, next);
				free(se);
				se = NULL;
				sa = (struct chunk*)malloc(sizeof(struct chunk));
				if(page %(1<<(a+1)) ==0){
					sa->start = page;
				}else{
					page = page-(1<<a);
					sa->start = page;
				}
				sa->order= a+1;
				TAILQ_INSERT_TAIL(&(buddy.chunks[a+1].chu),sa,next);
			}
			else break;
		}else{
			break;
		}
		a++;
	}

	return;

}


/**
 * Print out the order-@order free chunk list
 *
 *  In the example above, print_free_pages(0) will print out:
 *  0x1d:0
 *
 *  print_free_pages(2):
 *    0x10:2
 *    0x18:2
 */
void print_free_pages(const unsigned int order)
{
	//unsigned int starting_page = 0x43; /* I love 43 because it's perfect!! */

	/**
	 * Your implementation should print out each free chunk from the beginning
	 * in the following format.
	 * WARNING: USE fprintf(stderr) NOT printf, otherwise the grading
	 * system will evaluate your implementation wrong.
	 */
	struct chunk * temp;
	for (temp = buddy.chunks[order].chu.tqh_first; temp; temp = temp->next.tqe_next){
		fprintf(stderr, "    0x%x: %u\n", temp->start, temp->order);

	}
}


/**
 * Return the unusable index(UI) of order-@order.
 *
 * Description:
 *    Return the unusable index of @order. In the above example, we have 27 free
 *  pages;
 *  # of free pages =
 *    sum(i = 0 to @MAX_ORDER){ (1 << i) * # of order-i free chunks }
 *
 *    and
 *
 *  UI(0) = 0 / 27 = 0.0 (UI of 0 is always 0 in fact).
 *  UI(1) = 1 (for 0x1d) / 27 = 0.037
 *  UI(2) = (1 (0x1d) + 2 (0x1e-0x1f)) / 27 = 0.111
 *  UI(3) = (1 (0x1d) + 2 (0x1e-0x1f) + 4 (0x10-0x13) + 4 (0x18-0x1b)) / 27
 *        = 0.407
 *  ...
 */
double get_unusable_index(unsigned int order)
{
	//unsigned int order;
	struct chunk *temp = TAILQ_FIRST(&(buddy.chunks[order].chu));
	double total=0.0;
	double aa=0.0;
	int count=0;

	for(int i=0; i<NR_ORDERS; i++){
		count=0;
		for (temp = buddy.chunks[i].chu.tqh_first; temp; temp = temp->next.tqe_next){
			count++;
		}
		total+= (double)count*(1<<i);
	}

	temp = TAILQ_FIRST(&(buddy.chunks[order].chu));
	for(int i=0; i<order; i++){
		count=0;
		for (temp = buddy.chunks[i].chu.tqh_first; temp; temp = temp->next.tqe_next){
			count++;
		}
		aa += (double)count*(1<<i);
	}

	//fprintf(stderr, "!!! %lf\n", total);	
	return aa/total;
}


/**
 * Initialize your buddy system.
 *
 * @nr_pages_in_order: number of pages in order-n notation to manage.
 * For instance, if @nr_pages_in_order = 13, the system should be able to
 * manage 8192 pages. You can set @nr_pages_in_order by using -n option while
 * launching the program;
 * ./pa4 -n 13       <-- will initiate the system with 2^13 pages.
 *
 * Return:
 *   0      : On successful initialization
 *  -EINVAL : Invalid arguments or when something goes wrong
 */
int init_buddy(unsigned int nr_pages_in_order)
{
	int i;
	// struct chunk *ch;
	buddy.allocated = 0;
	buddy.free = 1 << nr_pages_in_order;
	int init = 1 << (nr_pages_in_order-MAX_ORDER);

	/* TODO: Do your initialization as you need */

	for (i = 0; i < NR_ORDERS; i++) {
		buddy.chunks[i].order = i;
		TAILQ_INIT(&(buddy.chunks[i].chu));

	}
	for(int j=0; j<init; j++){
		struct chunk *ch = (struct chunk *)malloc(sizeof(struct chunk));
		ch->order = MAX_ORDER;
		ch->start = j*(1<<MAX_ORDER);
		TAILQ_INSERT_TAIL(&(buddy.chunks[MAX_ORDER].chu), ch, next);
	//fprintf(stderr, "    j ===0x%x\n",TAILQ_NEXT(&(buddy.chunks[MAX_ORDER].chu),next)->start);
	}

	/**
	 * TODO: Don't forget to initiate the free chunk list with
	 * order-@MAX_ORDER chunks. Note you might add multiple chunks if
	 * @nr_pages_in_order > @MAX_ORDER. For instance, when
	 * @nr_pages_in_order = 10 and @MAX_ORDER = 9, the initial free chunk
	 * lists will have two chunks; 0x0:9, 0x200:9.
	 */

	return 0;
}


/**
 * Return resources that your buddy system has been allocated. No other
 * function will be called after calling this function.
 */
void fini_buddy(void)
{
	/**
	 * TODO: Do your finalization if needed, and don't forget to release
	 * the initial chunks that you put in init_buddy().
	 */
	struct chunk* temp = NULL;

	for(int i = 0;i < NR_ORDERS; i++){
		while(!TAILQ_EMPTY(&(buddy.chunks[i].chu))){
			temp = TAILQ_FIRST(&(buddy.chunks[i].chu));
			TAILQ_REMOVE(&(buddy.chunks[i].chu), temp, next);
			free(temp);
		}
	}
	return;
}
