#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

uint32_t shared_resource = 0;
mutex shres_guard;

constexpr uint32_t limit = INT_MAX;

void increment() {
    for (auto i = 0; i < limit; i++) {
        lock_guard<mutex> lock(shres_guard);
        ++shared_resource;
    }
    cout << "\nIncrement finished!" << endl;
}

int main() {
    jthread t1(increment);
    jthread t2(increment);

    t1.join();
    t2.join();
}
