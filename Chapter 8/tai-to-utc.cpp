#include <iostream>
#include <chrono>

using namespace std::chrono;

void tai_to_utc() {
    tai_time tai{tai_clock::now()};
    std::cout << "International atomic time (TAI): " << tai << '\n';
    utc_time utc{clock_cast<utc_clock>(tai)};
    std::cout << "Coordinated universal time (UTC): " << utc << '\n';
}

int main() {
    tai_to_utc();
    return 0;
}