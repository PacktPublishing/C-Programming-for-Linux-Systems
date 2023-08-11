#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
using namespace std;

mutex cv_mutex;
condition_variable cond_var;

void processing() {
    cout << "Processing shared resource." << endl;
}

void waiting() {
    cout << "Waiting for work..." << endl;

    unique_lock<mutex> lock(cv_mutex);
    cond_var.wait(lock);
    processing();
    cout << "Work done." << endl;
}

void done() {
    cout << "Shared resource ready."  << endl;
    cond_var.notify_one();
}

int main () {
    jthread t1(waiting);
    jthread t2(done);

    t1.join();
    t2.join();
    return 0;
}
