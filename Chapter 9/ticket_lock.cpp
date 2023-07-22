#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <new>
#include <thread>
#include <vector>

using std::hardware_constructive_interference_size;
using namespace std;

struct TicketLock {
    alignas(hardware_constructive_interference_size) 
        atomic_size_t serving = {0};
    alignas(hardware_constructive_interference_size) 
        atomic_size_t next = {0};

    void lock() {
        const auto ticket = next.fetch_add(1, 
                                memory_order_relaxed);

        while (serving.load(memory_order_acquire) != 
               ticket) {
            this_thread::yield();
        }
    }

    void unlock() {
        const auto next_thread = 
            serving.load(memory_order_relaxed) + 1;
        serving.store(next_thread, 
                      memory_order_release);
    }
};


TicketLock spinlock = {0};
vector<string> shared_res {};

void producer() {
    for(int i = 0; i < 100; i ++) {
        cout << "Producing: " << endl;
        spinlock.lock();
        shared_res.emplace_back("test1");
        shared_res.emplace_back("test2");
        spinlock.unlock();
        for (const auto& el : shared_res) 
            cout << "p:" << el << endl;
        this_thread::sleep_for(chrono::milliseconds(100));
    }
}

void consumer() {
    for(int i = 0; i < 100; i ++) {
        this_thread::sleep_for(chrono::milliseconds(100));
        cout << "Consuming: " << endl;
        for (const auto& el : shared_res)
            cout << "c:" << el << endl;
        spinlock.lock();
        shared_res.clear();
        spinlock.unlock();
        if (shared_res.empty()) cout << "Consumed" << endl;
    }
}

int main() {
    thread produce([]{ producer(); });
    thread consume([]{ consumer(); });
    produce.join();
    consume.join();
    return 0;
}