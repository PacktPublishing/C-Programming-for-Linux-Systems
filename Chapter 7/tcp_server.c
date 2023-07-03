#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT     8080
#define BUF_SIZE 256
#define BACKLOG  5

void exitHandler() {
    printf ("Exit command called - terminating server!\n");
    exit(EXIT_SUCCESSFUL);
}

int main() {
    signal(SIGINT, exitHandler);

    const char *ip 
        = "192.168.136.128";
    char buffer[BUF_SIZE]          = {0};
    int server_sock                = 0;
    int client_sock                = 0;
    struct sockaddr_in server_addr = {0};
    struct sockaddr_in client_addr = {0};
    socklen_t addr_size            = 0;
    int result                     = 0;

    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0){
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(ip);

    result = bind(server_sock, 
                  (struct sockaddr*)&server_addr, 
                  sizeof(server_addr));
    if (result < 0) {
      perror("Bind failed");
      exit(EXIT_FAILURE);
    }

    result = listen(server_sock, BACKLOG);
    if (result != 0) {
      perror("Server listen failed");
      exit(EXIT_FAILURE);
    }

    printf("Listening...\n");

    while(1) {
        addr_size = sizeof(client_addr);
        client_sock = 
            accept(server_sock, 
                   (struct sockaddr*)&client_addr, 
                   &addr_size);

        if (client_sock > 0) {
            printf("Client connected.\n");
            memset(buffer, 0, BUF_SIZE);
            result = recv(client_sock, 
                          buffer, 
                          sizeof(buffer), 
                          0);
            if (result > 0) {
                printf("Client request: %s\n", buffer);
                memset(buffer, 0, BUF_SIZE);
                sprintf(buffer, 
                        "The server is running by PID: %d", 
                        getpid());
                printf("Server response: %s\n", buffer);
                send(client_sock, 
                     buffer, 
                     strlen(buffer), 
                     0);
            }
            close(client_sock);
        }
        else {
            perror("Client connection failed");
        }
    }
    return 0;
}