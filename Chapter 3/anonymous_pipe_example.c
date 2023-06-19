#include <unistd.h>
#include <stdio.h>
#include <string.h>

#define BUFF_LEN ((size_t) 64)
#define pipeIn  0
#define pipeOut 1

int main() {
   int an_pipe[2];
   char buff[BUFF_LEN + 1] = {0};

   if (pipe(an_pipe) == 0) { // {1}
      int pid = fork();

      if (pid == 0) {
         sleep(1); // Just to give some extra time!
         close(an_pipe[pipeOut]); // {2}
         read(an_pipe[pipeIn], buff, BUFF_LEN); // {3}
         printf("Child: %s", buff);
      }
      else {
         close(an_pipe[pipeIn]); // {4}
         const char *msg = {"Sending message to child!"};
         write(an_pipe[pipeOut], msg, strlen(msg) + 1); // {5}
      }
   }

   return 0;
}