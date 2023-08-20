#include <thread>
#include <iostream>
#include <array>
#include <latch>
#include <syncstream>

using namespace std::literals::chrono_literals;

int main() {
    std::latch progress{2};
    std::array<std::jthread, 2> threads {
        std::jthread{[&](int num){
            std::osyncstream{std::cout} << "Starting thread " 
                                        << num 
                                        << " and go to sleep.\n";
            std::this_thread::sleep_for(100ms);
            std::osyncstream{std::cout} << "Decrementing the latch for thread " 
                                        << num << '\n';
            progress.count_down();
            std::osyncstream{std::cout} << "Thread " << num 
                                        << " finished!\n";
        }, 0},
        std::jthread{[&](int num){
            std::osyncstream{std::cout} << "Starting thread " 
                                        << num 
                                        << ". Arrive on latch and wait to become zero.\n";
            progress.arrive_and_wait();
            std::osyncstream{std::cout} << "Thread " << num << " finished!\n";
        }, 1}
    };

    std::osyncstream{std::cout} << "Main thread waiting workers to finish.\n";
    progress.wait(); // wait for all threads to finish.
    std::cout << "Main thread finished!\n";

    return 0;
}