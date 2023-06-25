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
        abort();
    }

    siginfo_t status = {0};
    waitid(P_PID, pids[1], &status, WEXITED);
    if (WIFSIGNALED(status))
        cout << "Child " << pids[1]
             << " aborted: "
             << "\nStatus update with SIGCHLD: "
             << status.si_signo
             << "\nTermination signal - SIGABRT: "
             << status.si_status
             << "\nTermination code - _exit(2): "
             << status.si_code << endl;
}

int main() {
    process_creator();
    return 0;
}