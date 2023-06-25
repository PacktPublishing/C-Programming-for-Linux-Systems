#include <iostream>
#include <stdlib.h>
#include <unistd.h>
using namespace std;
void process_creator() {

    if (fork() == 0) {
        cout << "Child process id: " << getpid() << endl;
        exit(EXIT_SUCCESS);
    }
    else {
        cout << "Parent process id: " << getpid() << endl;
    }
}

int main() {
    process_creator();
    return 0;
}