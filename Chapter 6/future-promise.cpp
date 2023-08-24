#include <future>
#include <thread>
#include <iostream>

using namespace std::literals::chrono_literals;

int main() {
    std::promise<std::string> promise;
    std::future<std::string> future{promise.get_future()}; // Get the future from the promise.

    std::jthread th1{[p{std::move(promise)}]() mutable {
        std::this_thread::sleep_for(20ms);
        p.set_value_at_thread_exit("I promised to call you back once I am ready!\n");
    }}; // Move the promise inside the worker thread.
    
    std::cout << "Main thread is ready.\n";
    std::cout << future.get(); // This is a blocking call!
    
    return 0;
}