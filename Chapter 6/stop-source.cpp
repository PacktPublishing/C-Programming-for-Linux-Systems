#include <iostream>
#include <syncstream>
#include <thread>
#include <array>

using namespace std::literals::chrono_literals;

int main() {
    std::stop_source source;
    const auto worker{[](std::stop_source sr, int num){
        std::stop_token token = sr.get_token();
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
        std::jthread{worker, source, 0},
        std::jthread{worker, source, 1},
        std::jthread{worker, source, 2}
    };

    std::this_thread::sleep_for(1s);
    source.request_stop(); // this is not a blocking call, it is just a request.
    
    std::osyncstream{std::cout} << "Main thread just requested stop!\n";

    return 0; // jthread dtors join them here.
}