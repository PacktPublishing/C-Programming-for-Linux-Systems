#include <iostream>
#include <unistd.h>
#include <string.h>

using namespace std;

constexpr auto BUFF_LEN = 64;
constexpr auto pipeIn   = 0;
constexpr auto pipeOut  = 1;

int main() {
   int a_pipe[2]{};
   char buff[BUFF_LEN + 1]{};

   if (pipe(a_pipe) == -1) {  // {1}
       perror("Pipe creation failed");
       exit(EXIT_FAILURE);
   }
   else {
      if (int pid = fork(); pid == -1) {
         perror("Process creation failed");
         exit(EXIT_FAILURE);
      }
      else if (pid == 0) {
         // Child: will be the reader!
         sleep(1); // Just to give some extra time!
         close(a_pipe[pipeOut]); // {2}
         read(a_pipe[pipeIn], buff, BUFF_LEN); // {3}
         cout << "Child: " << buff << endl;
      }
      else {
         // Parent: will be the writer!
         close(a_pipe[pipeIn]); // {4}
         const char *msg = {"Sending message to child!"};
         write(a_pipe[pipeOut], msg, strlen(msg) + 1); // {5}
      }
   }

   return 0;
}