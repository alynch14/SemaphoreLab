#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <unistd.h>

#define BUFF_SIZE 20

char buffer[BUFF_SIZE];
int nextIn = 0;
int nextOut = 0;

char sem_name1[] = "mutex";
char sem_name2[] = "empty_slots";
char sem_name3[] = "full_slots";
char sem_name4[] = "processed_slots";
sem_t *empty_slots;
sem_t *full_slots;
sem_t *mutex;
sem_t *processed_slots;

typedef struct {
char buffer[BUFF_SIZE];
int nextIn;
int nextProcessed;
int nextOut;
} shared_data;

shared_data *shm, *s;

void Get(char item)
{
  sem_wait(processed_slots);
  sem_wait(mutex);
  item = s->buffer[s->nextOut]; 
  s->nextOut = (s->nextOut + 1) % BUFF_SIZE;
  sem_post(mutex);
  printf("Process %d: Consuming %c ...\n", getpid(), item);
  sem_post(empty_slots);
}

void Consumer()
{
    int i;
    char  item;
    for(i = 0; i < 20; i++)
    {
      sleep(rand()%9);
      Get(item);
    }
}

void main()
{
	mutex=sem_open(sem_name1, O_CREAT,0644, 1);
	full_slots=sem_open(sem_name3, O_CREAT,0644, 0);
	empty_slots=sem_open(sem_name2, O_CREAT,0644, 10);
	processed_slots=sem_open(sem_name2, O_CREAT,0644, 0);


	//allocate the shared memory segment
	key_t key;
	key = 1234;

	//locate the segment
	int shmid;
	if ((shmid = shmget(key, sizeof(shared_data),0666)) <0)
	{
		perror("Shmget");
		exit(1);
	}
	//attach to the segment
	if ((shm = (shared_data *) shmat(shmid, NULL, 0))==(shared_data *) -1)
	{
		perror("Shmat");
		exit(1);
	}
	
	s=shm;
	s->nextOut = 0;	
	Consumer();
	shmdt((void *) shm);
 
}
