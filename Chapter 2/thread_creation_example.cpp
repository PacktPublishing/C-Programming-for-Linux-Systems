#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace std::chrono;
void detached_routine() {
    cout << "Starting detached_routine thread.\n";
    this_thread::sleep_for(seconds(2));
    cout << "Exiting detached_routine thread.\n";
}

void joined_routine() {
    cout << "Starting joined_routine thread.\n";
    this_thread::sleep_for(seconds(2));
    cout << "Exiting joined_routine thread.\n";
}

void thread_creator() {
    cout << "Starting thread_creator.\n";
    thread t1(detached_routine);
    cout << "Before - Is the detached thread joinable: "
         << t1.joinable() << endl;
    t1.detach();
    cout << "After - Is the detached thread joinable: "
         << t1.joinable() << endl;
    thread t2(joined_routine);
    cout << "Before - Is the joined thread joinable: "
         << t2.joinable() << endl;
    t2.join();
    cout << "After - Is the joined thread joinable: "
         << t2.joinable() << endl;
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Exiting thread_creator.\n";
}
 
int main() {
    thread_creator();
}