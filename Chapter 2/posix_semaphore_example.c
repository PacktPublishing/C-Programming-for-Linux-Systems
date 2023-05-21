#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>

#define COUNT_LIMIT 100

void* consumer(void* params);
void* producer(void* params);

unsigned int shared_resource = 0;
unsigned char full_flag = 0;
unsigned short counter = 0;

sem_t sem;

int main() {
    pthread_t tid_prod;
    pthread_t tid_cons;

    if (sem_init(&sem, 0, 1)) {
        perror("sem_init_failed!");
    }
    else {
        pthread_create(&tid_prod, NULL, producer, NULL);
        pthread_create(&tid_cons, NULL, consumer, NULL);

        pthread_join(tid_prod, NULL);
        pthread_join(tid_cons, NULL);

        sem_destroy(&sem);
        pthread_exit(NULL);
    }

    return 0;
}

void* consumer(void* params) {
    sem_post(&sem); //V(S)

    while (counter++ <= COUNT_LIMIT) {
        sem_wait(&sem); // P(S)
        printf("\nConsuming... ");
        if (1 == full_flag) {
            printf("value: %d\n", --shared_resource);
            full_flag = 0;
        }

        sem_post(&sem); // V(S)
        sched_yield(); // usleep(10)
    }

    sem_post(&sem);
    pthread_exit(0);
}

void* producer(void* params) {
    while (counter++ < COUNT_LIMIT) {
        sem_wait(&sem); // P(S)
        printf("\nProducing... ");
        if (0 == full_flag) {
            printf("value: %d\n", ++shared_resource);
            full_flag = 1;
        }

        sem_post(&sem); // V(S)
        sched_yield(); // usleep(10)
    }

    sem_post(&sem);
    pthread_exit(0);
}