#include <iostream>
#include <thread>
#include <atomic>

using namespace std;

atomic<uint16_t> shared_resource = 0;
atomic_flag condFlag{};

constexpr uint16_t limit = 100;

void increment() {
    for (auto i = 0; i < limit; i++) {
        condFlag.wait(true);
        condFlag.test_and_set();
        ++shared_resource;
        cout << shared_resource << " ";
        condFlag.notify_one();
    }
    cout << "\nIncrement finished!" << endl;
}

void decrement() {
    for (auto i = 0; i < limit; i++) {
        condFlag.wait(false);
        condFlag.clear();
        --shared_resource;
        cout << shared_resource << " ";
        condFlag.notify_one();
    }
    cout << "\nDecrement finished!" << endl;
}

int main() {
    condFlag.test_and_set();
    std::thread t1(increment);
    std::thread t2(decrement);

    t1.join();
    t2.join();
}
