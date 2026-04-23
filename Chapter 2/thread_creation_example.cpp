#include <chrono>
#include <iostream>
#include <sstream>
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
    stringstream msg;
    cout << "Starting thread_creator.\n";
    thread t1(detached_routine);
    msg << "Before - Is the detached thread joinable: " << t1.joinable()
        << endl;
    cout << msg.str();
    t1.detach();
    msg.str("");
    msg.clear();
    msg << "After - Is the detached thread joinable: " << t1.joinable() << endl;
    cout << msg.str();

    thread t2(joined_routine);
    msg.str("");
    msg.clear();
    msg << "Before - Is the joined thread joinable: " << t2.joinable() << endl;
    cout << msg.str();
    t2.join();
    msg.str("");
    msg.clear();
    msg << "After - Is the joined thread joinable: " << t2.joinable() << endl;
    cout << msg.str();
    this_thread::sleep_for(chrono::seconds(1));
    cout << "Exiting thread_creator.\n";
}

int main() { thread_creator(); }
