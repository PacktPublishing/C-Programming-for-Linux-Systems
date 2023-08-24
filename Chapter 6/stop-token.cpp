#include <iostream>
#include <syncstream>
#include <thread>
#include <array>

using namespace std::literals::chrono_literals;

int main() {
    const auto worker{[](std::stop_token token, int num){
        while (!token.stop_requested()) {
            std::osyncstream{std::cout} << "Thread with id " 
                                        << num 
                                        << " is currently working.\n"; 
            std::this_thread::sleep_for(200ms);
        }
        
        std::osyncstream{std::cout} << "Thread with id " 
                                    << num 
                                    << " is now stopped!\n";
    }};

    std::array<std::jthread, 3> my_threads{
        std::jthread{worker, 0},
        std::jthread{worker, 1},
        std::jthread{worker, 2}
    };

    // Give some time to the other threads to start executing ...
    std::this_thread::sleep_for(1s);

    // Let's stop them
    for (auto& thread : my_threads) {
        thread.request_stop(); // this is not a blocking call, 
                               // it is just a request.
    }

    std::osyncstream{std::cout} << "Main thread just requested stop!\n";

    // jthread dtors join them here.
    return 0;
}