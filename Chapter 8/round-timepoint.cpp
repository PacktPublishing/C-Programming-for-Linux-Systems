#include <iostream>
#include <chrono>

using namespace std::chrono;

void round_time_point() {
    seconds dur_sec_1{55s}; //{1}
    seconds dur_sec_2{65s}; //{2}
    minutes dur_min_1{round<minutes>(dur_sec_1)}; //{3}
    minutes dur_min_2{round<minutes>(dur_sec_2)}; //{4}
    std::cout << "Rounding up to " << dur_min_1 << '\n';
    std::cout << "Rounding down to " << dur_min_2 << '\n';
}

int main() {
    round_time_point();
    return 0;
}