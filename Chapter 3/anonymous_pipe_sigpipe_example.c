#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include<signal.h>

#define BUFF_LEN ((size_t) 64)
#define pipeIn  0
#define pipeOut 1

void handle_sigpipe(int sig) {
   printf("SIGPIPE handled!\n");
}

int main() {
   int a_pipe[2] = {0};
   char buff[BUFF_LEN + 1] = {0};

   if (pipe(a_pipe) == 0) { // {1}
      int pid = fork();

      if (pid == 0) {
         close(a_pipe[pipeOut]); // {2}
         close(a_pipe[pipeIn]);
      }
      else {
         struct sigaction act = {0};
         sigemptyset(&act.sa_mask);
         act.sa_handler = SIG_IGN; // {3}

         if(sigaction(SIGPIPE, &act, 0) == -1) {// {4}
            perror("sigaction");
            return (1);
         }
         close(a_pipe[pipeIn]);
         sleep(1);
         const char *msg = {"Sending message to child!"};
         write(a_pipe[pipeOut], msg, strlen(msg) + 1); // {5}
      }
   }

   return 0;
}