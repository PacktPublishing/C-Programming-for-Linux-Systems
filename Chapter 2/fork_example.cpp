#include <iostream>
#include <unistd.h>
#include <string_view>
using namespace std;
string_view message = "Process called with pid: ";
uint16_t global_res;
void process_creator() {

    if (fork() == 0) {
        cout << message << getpid() << endl;
        cout << "Child's global resource value: "
         << ++global_res << endl;     
    }
    else {
        cout << message << getpid() << endl;
        cout << "Parent's global resource value: "
         << ++global_res << endl;        
    }
}

int main() {
    process_creator();
    return 0;
}