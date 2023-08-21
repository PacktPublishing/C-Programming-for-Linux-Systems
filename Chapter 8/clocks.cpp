#include <iostream>
#include <chrono>

using namespace std::chrono;

void clocks() {
    tai_time tai{tai_clock::now()};
    utc_time utc{utc_clock::now()};
    std::cout << "International atomic time (TAI): " << tai << '\n';
    std::cout << "Coordinated universal time (UTC): " << utc << '\n';
}

int main() {
    clocks();
    return 0;
}