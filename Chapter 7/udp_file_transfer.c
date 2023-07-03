#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <arpa/inet.h>
#include <netinet/in.h>

#define PORT     8080
#define BUF_SIZE 256

void process_creator() {
    int sockfd                  = 0;
    char buffer[BUF_SIZE]       = {0};
    char response[]                  
        = "File received and stored!";
    char com_end[] = "ENDING MESSAGE";
    struct sockaddr_in servaddr = {0};
    struct sockaddr_in cliaddr  = {0};

    servaddr.sin_family = AF_INET; // {1]}
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (fork() == 0) {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        } // {2}

        if (bind(sockfd, 
                 (const struct sockaddr *)&servaddr,
                 sizeof(servaddr)) < 0 ) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        } // {3}

        socklen_t len = sizeof(cliaddr);

        int fd = open("new_test.dat", 
                      O_CREAT | O_WRONLY, 
                      0666); // {4}
        if(fd > 0) {
            while (1) {
                ssize_t bytes_received = 
                    recvfrom(sockfd, (char *)buffer, 
                             BUF_SIZE,
                             MSG_WAITALL,
                             (struct sockaddr *) &cliaddr,
                             &len); // {5}
                if (strncmp(com_end, 
                            buffer, 
                            strlen(com_end)) == 0) {
                    close(fd);
                    break; // {6}
                }
                else {
                    buffer[bytes_received] = '\0';
                    printf("Bytes received: %d\n", 
                           bytes_received);
                    write(fd, buffer, strlen(buffer));
                    memset(buffer, 0, sizeof(buffer));
                } // {7}
            }
        }
        sendto(sockfd, 
               (const char *)response, 
               strlen(response),
               MSG_CONFIRM, 
               (const struct sockaddr *) &cliaddr,
               len); // {8}

        exit(EXIT_SUCCESS);
    }
    else {
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) 
        {
            perror("Socket creation failed");
            exit(EXIT_FAILURE);
        } // {9}

        socklen_t len = 0;
        int fd = open("test.dat", O_RDONLY); // {10}
        if(fd > 0) {
            while(1) {
                ssize_t bytes_to_send = 
                    read(fd,
                         buffer,
                         BUF_SIZE); // {11}
                if(bytes_to_send > 0) {
                    buffer[bytes_to_send] = '\0';
                    printf("Bytes sent: %d\n", 
                           bytes_to_send);
                    sendto(sockfd,
                           (const char *)buffer,
                           bytes_to_send,
                           0,
                           (const struct sockaddr *) &servaddr,
                           sizeof(servaddr)); // {12}
                    memset(buffer, 0, sizeof(buffer));
                }
                else {
                    sendto(sockfd,
                           (const char *)com_end,
                           strlen(com_end),
                           0,
                           (const struct sockaddr *) &servaddr,
                           sizeof(servaddr)); // {13}
                    close(fd);
                    break;
                }
            }
        }
        ssize_t bytes_received = 
            recvfrom(sockfd, 
                     (char *)buffer, 
                     BUF_SIZE,
                     MSG_WAITALL, 
                     (struct sockaddr *) &servaddr,
                     &len); // {14}
        buffer[bytes_received] = '\0';
        printf("Server : %s\n", buffer);

        close(sockfd);
        exit(EXIT_SUCCESS);
    }
}

int main() {
    process_creator();
    return 0;
}
