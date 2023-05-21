#include <iostream>
#include <unistd.h>

using namespace std;

void process_creator() {

    if (execvp("./test_fork", NULL) == -1)
        cout << "Process creation failed!" << endl;
    else
        cout << "Process called!" << endl;
}

int main() {
    process_creator();
    return 0;
}