#include <iostream>
#include <thread>
#include <syncstream>
#include <array>

int main() {
    std::array<std::jthread, 5> my_threads; // Just an array of 5 jthread objects which do nothing.
    
    const auto worker{[]{ 
        const auto thread_id = std::this_thread::get_id();  // 3
        std::osyncstream sync_cout{std::cout};
        sync_cout << "Hello from new jthread with id:" << thread_id << '\n';
    }};

    for (auto& thread : my_threads) {
        thread = std::jthread{worker}; // This moves the new jthread on 
                                       // the place of the placeholder
    }

    std::osyncstream{std::cout} << "Hello Main program thread with id:"
                                << std::this_thread::get_id() 
                                << '\n';
    return 0; // jthread dtors join them here.
}