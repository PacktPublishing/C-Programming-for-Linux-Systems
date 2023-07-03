#include <stdio.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <semaphore.h>
#include <string.h>
#include <stdlib.h>

#define SHM_ID   "/test_shm"
#define SEM_ID   "/test_sem"
#define SHM_SIZE 1024

sem_t *sem;

void process_creator() {
    char buffer[SHM_SIZE] = {0};
    int res               = 0;
    int fd                = 0;
    void *shm_addr        = NULL;

    if (fork() == 0) {
        fd = shm_open(SHM_ID, O_RDONLY, 0700); // {1}

        if (fd == -1) {
            perror("Error opening shm region");
            exit(EXIT_FAILURE);
        }

        shm_addr = mmap(NULL, SHM_SIZE,
                        PROT_READ, MAP_SHARED,
                        fd, 0); // {2}
        if (shm_addr == MAP_FAILED) {
            perror("Error mapping shm region");
            exit(EXIT_FAILURE);
        }

        while(1) {
            sem_wait(sem);
            memcpy(buffer, shm_addr, SHM_SIZE); // {3}
            if(strlen(buffer) != 0) {
                printf("PID %d: consumed: \"%s\"\n", 
                       getpid(), buffer);
                break;
            }
            sem_post(sem);
        }

        exit(EXIT_SUCCESS);
    }
    else {
        int len = 0;

        fd = shm_open(SHM_ID, O_CREAT | O_RDWR, 0700); // {4}
        if (fd == -1) {
            perror("Error opening shm region");
            exit(EXIT_FAILURE);
        }

        res = ftruncate(fd, SHM_SIZE); // {5}
        if (res == -1) {
            perror("Error resizing shm region");
            exit(EXIT_FAILURE);
        }

        shm_addr = mmap(NULL, SHM_SIZE,
                        PROT_WRITE, MAP_SHARED,
                        fd, 0); // {6}
        if (shm_addr == MAP_FAILED) {
            perror("Error mapping shm region");
            exit(EXIT_FAILURE);
        }

        sem_wait(sem);
        sprintf(buffer, "Some test data, coming from: %d",
                getpid());
        len = strlen(buffer) + 1;
        memcpy(shm_addr, buffer, len); // {7}
        sem_post(sem);

        sleep(1); // {8}

        res = munmap(shm_addr, SHM_SIZE); // {9}
        if (res == -1) {
            perror("Error cleaning-up shm region");
            exit(EXIT_FAILURE);
        }

        fd = shm_unlink(SHM_ID); //{10}
        if (fd == -1) {
            perror("Error destructing shm region");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}

int main() {
    sem = sem_open(SEM_ID, O_CREAT, 0644, 0); // {11}
    process_creator();
    sem_close(sem); // {12}
    sem_unlink(SEM_ID);
    return 0;
}