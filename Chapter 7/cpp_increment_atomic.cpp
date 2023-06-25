#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

atomic<uint32_t> shared_resource = 0;
constexpr uint32_t limit = INT_MAX;

void increment() {
    for (auto i = 0; i < limit; i++) {
        ++shared_resource;
    }
    cout << "\nIncrement finished!" << endl;
}

int main() {
    std::thread t1(increment);
    std::thread t2(increment);

    t1.join();
    t2.join();
}
