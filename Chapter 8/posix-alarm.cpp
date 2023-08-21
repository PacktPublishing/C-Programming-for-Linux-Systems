#include <iostream>
#include <csignal>
#include <unistd.h>
#include <sys/time.h>
#include <atomic>

static std::atomic_bool continue_execution{true};

int main() {
    struct sigaction sa{};
    sa.sa_handler = [](int signum) {
        // Timer triggered, stop the loop.
        std::cout << "Timer expired. Stopping the task...\n";
        continue_execution = false;
    };
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGALRM, &sa, nullptr);

    // Configure the timer to trigger every 1 seconds
    struct itimerval timer{
        .it_interval{.tv_sec{1}, .tv_usec{0}}, 
        .it_value{.tv_sec{1}, .tv_usec{0}}
    };
    // Start the timer
    setitimer(ITIMER_REAL, &timer, nullptr);
    std::cout << "Timer started. Waiting for timer expiration...\n";
    // Keep the program running to allow the timer to trigger
    while (continue_execution) {
        sleep(1);
    }
    return 0;
}