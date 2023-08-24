#include <iostream>
#include <chrono>

using namespace std::chrono;

void time_epoch() {
    const time_point start{steady_clock::now()}; // {1}
    const duration epoch_to_start{start.time_since_epoch()}; // {2}
    std::cout << "Time since clock epoch: " << epoch_to_start << '\n'; // {3}
}

int main() {
    time_epoch();
    return 0;
}