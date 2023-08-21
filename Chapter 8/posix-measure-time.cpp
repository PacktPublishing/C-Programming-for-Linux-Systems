#include <iostream>
#include <ctime>

static const auto LIMIT{10000};

void dummy_f() {
    for (auto i{0}; i < LIMIT; ++i);
}
int main() {
    timespec start, end;
    // Start the timer
    clock_gettime(CLOCK_MONOTONIC, &start);
    // Measured code segment
    for (auto i{0}; i < LIMIT; ++i) {
        dummy_f();
    }
    // Stop the timer
    clock_gettime(CLOCK_MONOTONIC, &end);
    // Calculate the elapsed time
    const auto elapsed{(end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9};
    std::cout << "Elapsed time: " << elapsed << " seconds\n";
    return 0;
}