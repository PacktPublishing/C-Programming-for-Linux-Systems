#include <iostream>
#include <chrono>

using namespace std::chrono;

void system_clock_epoch() {
    time_point<system_clock> systemClockEpoch;
    std::cout << std::format("system_clock epoch: {0:%F}T{0:%R%z}.", systemClockEpoch) << '\n';
}

int main() {
    system_clock_epoch();
    return 0;
}