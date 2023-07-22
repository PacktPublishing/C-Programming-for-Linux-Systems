#include <iostream>
#include <condition_variable>
#include <mutex>
#include <thread>
using namespace std;

mutex _mutex;
condition_variable cond_var;

void processing(){
    cout << "Processing shared resource." << endl;
}

void waiting(){
    std::cout << "Waiting for work..." << std::endl;

    std::unique_lock<std::mutex> lock(_mutex);
    cond_var.wait(lock);
    processing();
    std::cout << "Work done." << std::endl;
}

void done(){
    std::cout << "Shared resource ready."  << std::endl;
    cond_var.notify_one();
}

int main (){
    std::thread t1(waiting);
    std::thread t2(done);

    t1.join();
    t2.join();
}
