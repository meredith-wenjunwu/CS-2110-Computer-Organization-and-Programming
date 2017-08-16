/*
 * CS 2110 Fall 2016
 * Author: Wenjun Wu
 */

/* we need this for uintptr_t */
#include <stdint.h>
/* we need this for memcpy/memset */
#include <string.h>
/* we need this for my_sbrk */
#include "my_sbrk.h"
/* we need this for the metadata_t struct definition */
#include "my_malloc.h"

/* You *MUST* use this macro when calling my_sbrk to allocate the
 * appropriate size. Failure to do so may result in an incorrect
 * grading!
 */
#define SBRK_SIZE 2048

/* If you want to use debugging printouts, it is HIGHLY recommended
 * to use this macro or something similar. If you produce output from
 * your code then you may receive a 20 point deduction. You have been
 * warned.
 */
#ifdef DEBUG
#define DEBUG_PRINT(x) printf(x)
#else
#define DEBUG_PRINT(x)
#endif

/* Our freelist structure - this is where the current freelist of
 * blocks will be maintained. failure to maintain the list inside
 * of this structure will result in no credit, as the grader will
 * expect it to be maintained here.
 * DO NOT CHANGE the way this structure is declared
 * or it will break the autograder.
 */
metadata_t* freelist;

/* traverse to find the available smallest block*/
metadata_t* traverseForAvailable(size_t size) {
    metadata_t* smallest = NULL;
    metadata_t* curr = freelist;
    while (curr != NULL) {
        /* Return the first block found that is equal/greater than size*/
        if (curr->block_size >= size) {
        	if (smallest == NULL || curr->block_size < smallest->block_size) {
            	smallest = curr;
        	}
        }
        curr = curr->next;
    }
    return smallest; // if no block is big enough, return NULL

}

/* traverse to find the appropriate palce and add the new block*/
void addtoFL(metadata_t* new) {
	if (freelist == NULL) {
		freelist = new;
		new-> next = NULL;
		return;
	}

	metadata_t* curr = freelist;
	/* Add block to first */
	if ((uintptr_t) new < (uintptr_t) curr) {
		new->next = curr;
		freelist = new;
		return;
	}
	/*add block in the middle */
	while (curr->next != NULL) {
		if ((uintptr_t)new < (uintptr_t)(curr->next)) { // compare address
			new->next = curr->next;
			curr->next = new; 
			return;
		}
		curr = curr->next;
	}
	/* if curr is the last block in list */
		
	curr -> next = new;
	new->next = NULL;
}

void removeFromFL(metadata_t* toremove) {
	metadata_t* curr = freelist;
	/* Remove the first block */
	if (curr == toremove) {
		freelist = curr -> next;
		toremove->next = NULL;
		return;
	}

	/* Remove the block in the middle */
	while (curr -> next != NULL) {
		if (curr -> next == toremove) {
			curr -> next = (curr -> next) -> next;
			toremove->next = NULL;
			return;
		}
		curr = curr->next;
	}
}


/* split the given block from the front */
metadata_t* splitFL(metadata_t* curr, size_t size) {
	unsigned int c;
	//remove the block to split from Freelist
	removeFromFL(curr);
	//create a new pointer point to the start of the second block (split from front)
	metadata_t* new = (metadata_t*)((char*)curr + size);
	//change parameters
	size_t new_size = curr->block_size - size;
	new->block_size = new_size;
	curr->next = NULL;
	new->canary = 0;
	new->request_size = 0;
	//add the modified block back to Freelist
	addtoFL(new);

	//modified the original pointer, add canary and return back to user
	curr->block_size = size;
	curr->request_size = size - sizeof(unsigned int) - sizeof(metadata_t);
	curr ->next = NULL;
	c = ((curr->block_size<<16)|curr->request_size);
	curr->canary = (c^(int)(uintptr_t)curr); //canary
	*(int*)(((char*)curr) + curr->request_size + sizeof(metadata_t)) = (c^(int)(uintptr_t)curr);
	return curr;

}

void* my_malloc(size_t size) {
    size_t nsize = size + sizeof(unsigned int) + sizeof(metadata_t);
    if (nsize > 2048) {
    	ERRNO = SINGLE_REQUEST_TOO_LARGE;
    	return NULL;
    }
    if (nsize > 4*2048) {
    	ERRNO = OUT_OF_MEMORY;
    	return NULL;
    }
    // if this is the 1st time calling my_malloc
    if (freelist == NULL) {
    	freelist = my_sbrk(SBRK_SIZE);
    	if (freelist == NULL) {
            
            /* sbrk returned NULL, meaning out of mem */
            ERRNO = OUT_OF_MEMORY;
            return NULL;
        }
    	freelist -> next = NULL;
    	freelist -> block_size = 2048;
    	freelist -> request_size = 0;
    	freelist -> canary = 0;
    }

    // find the first large enough block
    metadata_t* available = traverseForAvailable(nsize);
    
    // if no available found, create a new block
    if (available == NULL) {
    	metadata_t* new = my_sbrk(SBRK_SIZE);
    	if (new == NULL) {
    		ERRNO = OUT_OF_MEMORY;
    		return NULL;
    	}
    	new -> next = NULL;
    	new -> block_size = 2048;
    	new -> request_size = 0;
    	new -> canary = 0;
    	addtoFL(new);
    	available = new;
    }
    // remove the entire block or split the block according to requested size
    if (available->block_size==nsize || available->block_size < nsize+sizeof(unsigned int)+sizeof(metadata_t) + 1) {
    	removeFromFL(available);
    	available -> request_size = size;
		unsigned int c = ((available->block_size<<16)|available->request_size);
		available->canary = (c ^ (int)(uintptr_t)available);
		*(int*)(((char*)available) + size + sizeof(metadata_t)) = available ->canary;
    	ERRNO = NO_ERROR;
    	return (available + 1);
    }
    metadata_t* removed = splitFL(available, nsize);
	ERRNO = NO_ERROR;
	return (removed + 1);
}


void* my_realloc(void* ptr, size_t new_size) {
    if (!ptr) {
    	// NULL ptr, should act like malloc
    	return my_malloc(new_size);
    }
    if (new_size == 0) {
    	//new_size = 0; should act like free
    	my_free((metadata_t*)ptr);
    	return NULL;
    }
    // remalloc, copy over data and 
    metadata_t* block = (metadata_t*)ptr - 1;

   if (block->request_size == new_size) {
   		return ptr;
   }
	metadata_t* new_block = my_malloc(new_size);
	memcpy(new_block, (block+1), new_size);
	my_free(block + 1);
	return new_block;
} 

void* my_calloc(size_t nmemb, size_t size) {
    size_t s = nmemb*size;
    if (s > 2048)
	{
		ERRNO = SINGLE_REQUEST_TOO_LARGE;
		return NULL;
	}
	void* array = my_malloc(s);
	size_t i;
	for (i = 0; i < s; i++) {
		*((char*)array + i) = 0;
	}
	ERRNO = NO_ERROR;
	return array;
}

/* Merge two blocks by modifying the first block */
metadata_t* merge(metadata_t* block1, metadata_t* block2) {
	size_t new_size = block1->block_size + block2->block_size;
	block1->block_size = new_size;
	block1->next = NULL;
	block1->canary = 0;
	block1->request_size = 0;
	ERRNO = NO_ERROR;
	return block1;
}

void my_free(void* ptr) {
	if (ptr == NULL) {
		return;
	}
	metadata_t* block = ((metadata_t*) ptr) - 1;
	size_t rs = block->request_size;
	size_t bs = block->block_size;
	unsigned int c = ((bs<<16)|rs);
	c = (c ^ (int)(uintptr_t)block);
	if (block->canary != c) {
		ERRNO = CANARY_CORRUPTED;
		return;
	}
	if (*(int*)(((char*)block) + rs+ sizeof(metadata_t)) != c) {
		ERRNO = CANARY_CORRUPTED;
		return;
	}

	block->canary = 0;
	block->next = NULL;
	block->request_size = 0;

	if (freelist == NULL) {
		freelist = block;
		ERRNO = NO_ERROR;
		return;

	} else {
		//serch through FL to find where to insert and try to merge
		metadata_t* curr = freelist;
		if (((char*)block) + bs ==(char*)curr) {
			freelist = curr->next; //delete curr from FL;
			metadata_t* merged = merge(block, curr);
			addtoFL(merged);
			ERRNO = NO_ERROR;
			return;

		} else if (((char*)block) + bs < (char*)curr) {
			block->next = curr;
			freelist = block;
			ERRNO = NO_ERROR;
			return;
		}

		// if the place that this block could fit is in the middle of the free list
		while(curr->next != NULL) {
			if ((((char*)curr) + curr->block_size == (char*)block) && (((char*)block) + bs == (char*)curr->next)) {
				// merge with two blocks in the free list
				metadata_t* next = curr->next;
				removeFromFL(curr);
				removeFromFL(next);
				metadata_t* merged = merge(curr,block);
				merged = merge(merged,next);//merge two times
				addtoFL(merged);
				ERRNO = NO_ERROR;
				return;
			} else if (((char*)curr) + curr->block_size == (char*)block) {
				//merge with front
				removeFromFL(curr);
				metadata_t* merged = merge(curr,block);
				addtoFL(merged);
				ERRNO = NO_ERROR;
				return;
			} else if (((char*)block) + bs == (char*)curr->next) {
				//merge with back
				metadata_t* next = curr->next;
				removeFromFL(next);
				metadata_t* merged = merge(block, next);
				addtoFL(merged);
				ERRNO = NO_ERROR;
				return;
			} else if ((((char*)curr) + curr->block_size < (char*)block) && (((char*)block)+ bs < (char*)curr->next)) {
				//No merge, insert the block
				block->next = curr->next;
				curr->next = block;
				ERRNO = NO_ERROR;
				return;
			}
			curr = curr->next;
		}
		// if the block needs to merge with the end of the last block
		if (((char*)curr) + curr->block_size == (char*)block) {
			removeFromFL(curr);
			metadata_t* merged = merge(curr, block);
			addtoFL(merged);
			ERRNO = NO_ERROR;
			return;
		}
		
		// insert the block at the end if the 
		//block does not fit in all positions searched
		curr->next = block;
		ERRNO = NO_ERROR;
		return;

	}



}

