#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#define  MAX_WAITING 3
#define  MAX_STUDENTS_PROGRAMMING 10

int waiting_queue[MAX_WAITING] = {0, 0, 0};
int nextToWait = 0;
int nextToTutor = 0;
int studentNumber = 0;

sem_t mutex;
sem_t students_waiting;
sem_t currently_tutoring;
sem_t ready_to_tutor;


int Get_Tutored(int id){
        sem_wait(&students_waiting);
        sem_wait(&mutex);
        waiting_queue[nextToWait] = id;
        nextToWait = (nextToWait +1) % MAX_WAITING;
        sem_post(&mutex);
        printf("Student with id: %d is waiting ...\n", id);
        sem_wait(&currently_tutoring);
        printf("Student %d is being tutored ... \n", id);
        sem_post(&students_waiting);
        sem_post(&ready_to_tutor);
        return 1;
}

void * student(){
    int have_i_been_helped = 0;
    int id = ++studentNumber;
    
    while(have_i_been_helped == 0){
        int openWaitingSlots;
        sem_getvalue(&students_waiting, &openWaitingSlots);
        if(openWaitingSlots == 0){
            sleep(rand()%10);
            printf("Student with id: %d is programming ...\n", id);
        }
        else{
            have_i_been_helped = Get_Tutored(studentNumber);
        }
    }
}

void Start_Tutoring(){
    sem_wait(&ready_to_tutor);
    sem_wait(&mutex);
    int student = waiting_queue[nextToTutor];
    nextToTutor = (1+nextToTutor) % MAX_WAITING;
    sem_post(&mutex);
    printf("Tutor is currently helping student %d ...\n", student);
    sleep(rand() % 10);
    sem_post(&currently_tutoring);
}

void * tutor(){
    int currentlyprogramming;
    
    for(int i = 0; i < MAX_STUDENTS_PROGRAMMING; i++){
        
        int currentlyWaiting;
        sem_getvalue(&students_waiting, &currentlyWaiting);
        if(currentlyWaiting < 3){
            Start_Tutoring();
        }
        else{
            printf("Tutor is currently sleeping ...\n");
            sleep(rand()%6);
        }
    }
}

int main(){
    sem_init(&students_waiting, 0, MAX_WAITING);
    sem_init(&currently_tutoring, 0, 1);
    sem_init(&mutex, 0, 1);
    sem_init(&ready_to_tutor, 0, 1);

	pthread_t sid1, sid2, sid3, sid4, sid5, sid6, sid7, sid8, sid9, sid10, tutorId;
    pthread_create(&sid1, NULL, student, NULL);
    pthread_create(&sid2, NULL, student, NULL);
    pthread_create(&sid3, NULL, student, NULL);
    pthread_create(&sid4, NULL, student, NULL);
    pthread_create(&sid5, NULL, student, NULL);
    pthread_create(&sid6, NULL, student, NULL);
    pthread_create(&sid7, NULL, student, NULL);
    pthread_create(&sid8, NULL, student, NULL);
    pthread_create(&sid9, NULL, student, NULL);
    pthread_create(&sid10, NULL, student, NULL);
    pthread_create(&tutorId, NULL, tutor, NULL);
    pthread_join(sid1, NULL);
    pthread_join(sid2, NULL);
    pthread_join(sid3, NULL);
    pthread_join(sid4, NULL);
    pthread_join(sid5, NULL);
    pthread_join(sid6, NULL);
    pthread_join(sid7, NULL);
    pthread_join(sid8, NULL);
    pthread_join(sid9, NULL);
    pthread_join(sid10, NULL);
    pthread_join(tutorId, NULL);

    return 0;
}

