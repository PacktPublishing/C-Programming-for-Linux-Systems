#include <atomic>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <new>
#include <syncstream>
#include <thread>
#include <vector>

using std::hardware_destructive_interference_size;
using namespace std;
using namespace std::literals::chrono_literals;

struct TicketLock {
    alignas(hardware_destructive_interference_size) 
        atomic_size_t serving;
    alignas(hardware_destructive_interference_size) 
        atomic_size_t next;

    void lock() {
        const auto ticket = next.fetch_add(1, 
                                memory_order_relaxed);

        while (serving.load(memory_order_acquire) != 
               ticket) {// Consider this_thread::yield() 
                        // for excessive iterations, which
                        // go over a given threshold.
        }

    }

    void unlock() {
        serving.fetch_add(1, memory_order_release);
    }
};

TicketLock spinlock = {0};
vector<string> shared_res {};

void producer() {
    for(int i = 0; i < 100; i ++) {
        osyncstream{cout} << "Producing: " << endl;
        spinlock.lock();
        shared_res.emplace_back("test1");
        shared_res.emplace_back("test2");
        for (const auto& el : shared_res) 
            osyncstream{cout} << "p:" << el << endl;
        spinlock.unlock();
        this_thread::sleep_for(100ms);
    }
}

void consumer() {
    for(int i = 0; i < 100; i ++) {
        this_thread::sleep_for(100ms);
        osyncstream{cout} << "Consuming: " << endl;
        spinlock.lock();
        for (const auto& el : shared_res)
            osyncstream{cout} << "c:" << el << endl;
        shared_res.clear();
        spinlock.unlock();
        if (shared_res.empty()) 
            osyncstream{cout} << "Consumed" << endl;
    }
}

int main() {
    jthread produce([]{ producer(); });
    jthread consume([]{ consumer(); });
    produce.join();
    consume.join();
    return 0;
}