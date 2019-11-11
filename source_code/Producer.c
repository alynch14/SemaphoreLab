#include <stdio.h>
#include <sys/types.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>

#define BUFF_SIZE 20


typedef struct {
char buffer[BUFF_SIZE];
int nextIn;
int nextProcessed;
int nextOut;
} shared_data;

shared_data *shm, *s;
char sem_name1[] = "mutex";
char sem_name2[] = "empty_slots";
char sem_name3[] = "full_slots";
char sem_name4[] = "processed_slots";

sem_t *empty_slots;
sem_t *full_slots;
sem_t *mutex;
sem_t *processed_slots;

void Put(char item)
{
  sem_wait(empty_slots);
  sem_wait(mutex);
  s->buffer[s->nextIn] = item;
  s->nextIn = (s->nextIn + 1) % BUFF_SIZE;
  sem_post(mutex);
  printf("Process %d: Producing %c ...\n", getpid(), item);
  sem_post(full_slots);
}

void Producer()
{
    int i;

    for(i = 0; i < 20; i++)
    {
      sleep(rand()%3);
      Put((char)('A'+ i % 26));
    }
}


void main()
{
	mutex=sem_open(sem_name1, O_CREAT,0644, 1);
	full_slots=sem_open(sem_name3, O_CREAT,0644, 0);
	empty_slots=sem_open(sem_name2, O_CREAT,0644, 10);
	processed_slots=sem_open(sem_name4, O_CREAT,0644, 10);

	//allocate the shared memory segment
	key_t key;
	key = 1234;
	//create the segment
	int shmid;
	
	if ((shmid = shmget(key, sizeof(shared_data), IPC_CREAT |0666)) <0)
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
	s->nextIn = 0;
	 	
	Producer( );
        //detach
	shmdt((void *) shm);
}
