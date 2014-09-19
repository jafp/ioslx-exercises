/*
 * Copyright (c) 2013 Jacob A. F. Pedersen and Andre D. B. Christensen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <signal.h>
#include <semaphore.h>

#define SIZE		3
#define ITERATIONS	10

/**
 *  Counter to create incrementing numbers for each item
 */
int counter = 0;

/**
 * Index for next insert operation
 */
int ins_idx = 0;

/**
 * Index for next remove operation
 */
int rm_idx = 0;

/**
 * Fixed shared buffer of items
 */
int buffer[SIZE] = {0};

/** 
 * Semaphore for counting number of occupied slots in the buffer
 */
sem_t full;

/**
 * Semaphore for counting number of empty slots in the buffer.
 */
sem_t empty;

/**
 * Semaphore for mutual exclusion of the remove and insert operations.
 */
sem_t mutex;

/**
 * Insert item into buffer
 */
void insert_item(int item)
{
	buffer[ins_idx++] = item;
	if (ins_idx >= SIZE)
	{
		ins_idx = 0;
	}
}

/**
 * Remove item from buffer
 */
int remove_item()
{
	int item = buffer[rm_idx++];
	if (rm_idx >= SIZE)
	{
		rm_idx = 0;
	}
	return item;
}

/**
 * `Producer`thread
 */
void * producer(void * ptr)
{
	int i, item;
	for (i = 0; i < ITERATIONS; i++)
	{
		item = ++counter;
		sem_wait(&empty);
		sem_wait(&mutex);

		printf("Produce item 	%3d\n", item);
		insert_item(item);

		sem_post(&mutex);
		sem_post(&full);

		usleep(2000);
	}
	pthread_exit(0);
}

/**
 * `Consumer`thread
 */
void * consumer(void * ptr)
{
	int i, item;
	for (i = 0; i < ITERATIONS; i++)
	{
		sem_wait(&full);
		sem_wait(&mutex);

		item = remove_item();
		printf("Consumed item 	%3d\n", item);

		sem_post(&mutex);
		sem_post(&empty);

		usleep(0);
	}
	pthread_exit(0);
}

/**
 * Main
 */
int main(int argc, char ** argv)
{
	pthread_t pro, con;

	printf("\nProducer-consumer simulation\n");
	printf("Using Pthreads and POSIX Semaphores\n\n");

	sem_init(&full, 0, 0);
	sem_init(&empty, 0, SIZE);
	sem_init(&mutex, 0, 1);

	pthread_create(&pro, NULL, producer, NULL);
	pthread_create(&con, NULL, consumer, NULL);

	pthread_join(pro, NULL);
	pthread_join(con, NULL);

	sem_destroy(&full);
	sem_destroy(&empty);
	sem_destroy(&mutex);

	printf("\nDone. Resources destroyed.\n");

	return 0;
}