#include <condition_variable>
#include <iostream>
#include <mutex>
#include <thread>
#include <syncstream>

using namespace std;
 
int main() {
    osyncstream{cout} << "Main thread id: " 
                      << this_thread::get_id()
                      << endl;
    jthread worker{[](stop_token stoken) { // 1 
        mutex mutex;
        unique_lock lock(mutex);
        condition_variable_any().wait(lock, stoken,
            [&stoken] { return stoken.stop_requested(); });
        osyncstream{cout} << "Thread with id " 
                          << this_thread::get_id() 
                          << " is currently working."
                          << endl;
    }}; 
 
    stop_callback callback(worker.get_stop_token(), [] {
        osyncstream{cout} << "Stop callback executed by thread: "
                          << this_thread::get_id() 
                          << endl;
    });
 
    auto stopper_func = [&worker] {
        if (worker.request_stop())
            osyncstream{cout} << "Stop request executed by thread: "
                              << this_thread::get_id() 
                              << endl;
    };
 
    jthread stopper(stopper_func);
    stopper.join();
}