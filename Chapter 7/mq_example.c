#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

#define MAX_SIZE    1024
#define QUEUE_NAME  "/test_queue"

#define READER      0
#define WRITER      1

#define NUM_THREADS 2

void *readFromQueue(void *args) {
    printf("Thread READER starting...\n");

    mqd_t          mq               = {0};
    struct mq_attr attr             = {0};
    char           buffer[MAX_SIZE] = {0};
    ssize_t        bytes_read       = 0;

    /* initialize the queue attributes */
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    /* create the message queue */
    mq = mq_open(QUEUE_NAME, O_CREAT | 
                 O_RDONLY, 0700, &attr); // {1}

    /* check if the queue open is succesful */
    if (mq > -1) {
        while(1) {
            /* receive the message */
            bytes_read = mq_receive(mq, buffer,
                                    MAX_SIZE, 
                                    NULL); // {2}

            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                printf("Received: %s\n", buffer); // {3}
            }
            else {
                printf("\n\n\n***Receiving ends***\n");
                mq_close(mq); // {4}
                break;
            }
         }
    }
    else {
        printf("Receiver: Failed to load queue: %s\n", 
               strerror(errno));
    }
    
    mq_unlink(QUEUE_NAME);
    pthread_exit(NULL);
}

void *writeToQueue(void *args) {
    printf("Thread WRITER starting...\n");

    mqd_t mq               = {0};
    char  buffer[MAX_SIZE] = {0}; // This could be taken from stdin

    /* open the queue */
    mq = mq_open(QUEUE_NAME, O_WRONLY, 0700, NULL); // {5}

    /* check if the queue open is succesful */
    if (mq > -1)  {
        int fd = open("test.dat", O_RDONLY); // {6}
        if(fd > 0) {
            while(1) {
                ssize_t bytes_to_send = 
                    read(fd, buffer,
                         sizeof(buffer)); // {7}
                if(bytes_to_send > 0) {
                    mq_send(mq, buffer, MAX_SIZE, 0); // {8}
                }
                else {
                    printf("Sending ends.... \n");
                    mq_send(mq, buffer, 0, 0); // {9}

                    break;
                }

                memset(buffer, 0, sizeof(buffer));
                bytes_to_send = 0;
            }
        }
        else {
            printf("Error opening file\n");
        }
        close(fd); // {10}
        mq_close(mq);
    }
    else {
        printf("Sender: Failed to load queue: %s!\n", 
               strerror(errno));
    }

    pthread_exit(NULL);
}

int main (int argc, char *argv[]) {
    pthread_t      thread[NUM_THREADS] = {0};
    pthread_attr_t attr                = {0};
    int            result              = 0;
    int            thr_count           = 0;
    void           *status             = NULL;

    /* initialize attributes */
    pthread_attr_init(&attr);

    result = pthread_create(&thread[READER], &attr, readFromQueue, NULL); // {12}
    if (0 != result) {
        printf("ERROR; return code from pthread_create() is %d\n", result);
    }

    result = pthread_create(&thread[WRITER], &attr, writeToQueue, NULL); // {13}
    if (0 != result) {
        printf("ERROR; return code from pthread_create() is %d\n", result);
    }

    /* free attribute and wait for the other threads */
    pthread_attr_destroy(&attr);

    result = pthread_join(thread[WRITER], &status); // {14}
    if (0 != result) {
        printf("ERROR; return code from pthread_join() is %d\n", result);
    }

    result = pthread_join(thread[READER], &status); // {15}
    if (0 != result) {
        printf("ERROR; return code from pthread_join() is %d\n", result);
    }

    printf("Main: program completed. Exiting.\n");
    pthread_exit(NULL);

    return 0;
}
