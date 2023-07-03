#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT     8080
#define BUF_SIZE 256
#define BACKLOG  5

int main(){
    const char *ip 
        = "192.168.136.128";

    int sock = 0;
    struct sockaddr_in addr = {0};
    socklen_t addr_size = 0;
    char buffer[BUF_SIZE] = {0};
    int result = 0;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = inet_addr(ip);

    connect(sock, (struct sockaddr*)&addr, sizeof(addr));

    sprintf(buffer, 
            "The client is running with PID: %d", 
            getpid());
    printf("Client request: %s\n", buffer);
    send(sock, buffer, strlen(buffer), 0);

    memset(buffer, 0, BUF_SIZE);
    result = recv(sock, buffer, sizeof(buffer), 0);
    if (result > 0) {
        printf("Server response: %s\n", buffer);
    }

    close(sock);
    printf("Job done! Disconnecting.\n");
    return 0;
}