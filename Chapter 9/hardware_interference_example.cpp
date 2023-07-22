#include <atomic>
#include <chrono>
#include <iostream>
#include <new>
#include <thread>

using namespace std;
using std::hardware_constructive_interference_size;

void increment(std::atomic<uint32_t>& shared_res) {
    for(int i = 0; i < 100000; ++i) {shared_res++;}
}

int main() {
    auto start = chrono::steady_clock::now();
    alignas(hardware_constructive_interference_size) 
        atomic<uint32_t> a_var1 = 0;
    alignas(hardware_constructive_interference_size) 
        atomic<uint32_t> a_var2 = 0;
    alignas(hardware_constructive_interference_size) 
        atomic<uint32_t> a_var3 = 0;

    std::thread t1([&]() {increment(a_var1);});
    std::thread t2([&]() {increment(a_var2);});
    std::thread t3([&]() {increment(a_var3);});

    t1.join();
    t2.join();
    t3.join();
    auto end = chrono::steady_clock::now();
    cout << "Elapsed time in microseconds: "
        << chrono::duration_cast<chrono::microseconds>(end - start).count()
        << " µs" << endl;

    cout << "L1 Cache Line size: "
         << hardware_constructive_interference_size 
         << " bytes" << endl;

    cout << "The atomic var size is: " << sizeof(a_var1)
              << " and the addresses are: \n" 
              << &a_var1 << endl
              << &a_var2 << endl
              << &a_var3 << endl;

    return 0;
}