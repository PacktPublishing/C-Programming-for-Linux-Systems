#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define FIFO_NAME "example_fifo"
#define BUFF_LEN ((size_t) 64)

int main() {
    pid_t childId = 0;
    int named_pipe_fd = 0;

    mkfifo(FIFO_NAME, 0666); // {1}
    childId = fork();

    if (0 == childId) {
        named_pipe_fd = open(FIFO_NAME, O_RDWR); // {2}

        if (0 <= named_pipe_fd) {
            char message[BUFF_LEN] = {0};
            sleep(1);

            size_t bytes_recv =
                read(named_pipe_fd,
                     message,
                     sizeof(message)); // {3}

            if(bytes_recv > 0) {
                const char response_msg[]
                    = "Child printed the message!\n";
                printf("Child: %s\n", message);
                write(named_pipe_fd,
                      response_msg,
                      sizeof(response_msg)); // {4}
                close(named_pipe_fd); // {5}
            }
        }
        else {
            printf("Child cannot open the pipe!\n");
        }
    }
    else if (0 < childId) {
        named_pipe_fd = open(FIFO_NAME, O_RDWR); // {6}
        char message[BUFF_LEN] 
            = "Sending some message to the child!";
        write(named_pipe_fd,
              message,
              strlen(message) + 1); // {7}

        sleep(1);
        memset(message, 0, 
               strlen(message) + 1); // {8}
        read(named_pipe_fd, 
             message, 
             sizeof(message)); // {9}
        printf("Parent: %s", message);
        close(named_pipe_fd); // {10}
    }
    else {
        printf("Fork failed!");
    }

    //unlink(FIFO_NAME); // {11}
    return 0;
}