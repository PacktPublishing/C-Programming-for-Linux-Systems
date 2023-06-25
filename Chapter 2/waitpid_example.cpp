#include <iostream>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
using namespace std;
void process_creator() {

    pid_t pids[2] = {0};
    if ((pids[0] = fork()) == 0) {
        cout << "Child process id: " << getpid() << endl;
        exit(EXIT_SUCCESS);
    }
    if ((pids[1] = fork()) == 0) {
        cout << "Child process id: " << getpid() << endl;
        exit(EXIT_FAILURE);
    }

    int status = 0;
    waitpid(pids[0], &status, 0);
    if (WIFEXITED(status))
        cout << "Child " << pids[0]
             << " terminated with: "
             << status << endl;

    waitpid(pids[1], &status, 0);
    if (WIFEXITED(status))
        cout << "Child " << pids[1]
             << " terminated with: "
             << status << endl;
}

int main() {
    process_creator();
    return 0;
}