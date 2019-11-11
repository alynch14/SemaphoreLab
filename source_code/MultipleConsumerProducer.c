#include <pthread.h> 
#include <stdio.h> 
#include <semaphore.h>
#define BUFF_SIZE 20 

char buffer[BUFF_SIZE]; 
int nextIn = 0;

int nextOut = 0;

//used for synchronization – producer should be blocked if the buffer is full
sem_t empty_slots;

//used for synchronization – consumer should be blocked if the buffer is empty 
sem_t full_slot;

//used for mutual exclusion – enforces mutual exclusion on the shared buffer 
sem_t mutex;

void Put(char item) {
	sem_wait(&empty_slots);
	sem_wait(&mutex);
	buffer[nextIn] = item;
	nextIn = (nextIn + 1) % BUFF_SIZE; 
	sem_post(&mutex);
	printf("Thread with id: %lu Producing %c ...\n", pthread_self(),  item);
	sem_post(&full_slot);
}

void * Producer() {
	int i;
	for(i = 0; i < 15; i++) {
		sleep(rand()%6);
		Put((char)('A'+ i % 26)); 
	}
}

void Get(char item) {
	sem_wait(&full_slot);
	sem_wait(&mutex);
	item = buffer[nextOut];
	nextOut = (nextOut + 1) % BUFF_SIZE; 
	sem_post(&mutex);
	printf("Thread with id: %lu Consuming %c ...\n", pthread_self(), item);
	sem_post(&empty_slots);
}

void * Consumer() {
	int i;
	char item;
	for(i = 0; i < 15; i++) {
		sleep(rand()%6);
		Get(item); }
}
void main() {
	/* Initialize semaphores */
	sem_init(&empty_slots, 0, BUFF_SIZE);
	sem_init(&full_slot, 0, 0);
	sem_init(&mutex, 0, 1);

	pthread_t pid, cid, pid2, cid2, pid3, cid3;
	pthread_create(&pid, NULL, Producer, NULL); 
	pthread_create(&cid, NULL, Consumer, NULL);
	// create more consumer & producer threads.
	pthread_create(&pid2, NULL, Producer, NULL);
	pthread_create(&cid2, NULL, Consumer, NULL);
	pthread_create(&pid3, NULL, Producer, NULL);
	pthread_create(&cid3, NULL, Consumer, NULL);
	pthread_join(pid, NULL); 
	pthread_join(cid, NULL);
	pthread_join(pid2, NULL);
	pthread_join(cid2, NULL);
	pthread_join(pid3, NULL);
	pthread_join(cid3, NULL);
}
