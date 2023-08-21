#include <iostream>
#include <chrono>

using namespace std::chrono;

void duration_cast() {
    // const minutes dur_minutes{steady_clock::now().time_since_epoch()}; // doesn't compile
    auto dur_from_epoch{steady_clock::now().time_since_epoch()}; // {1}
    const minutes dur_minutes{duration_cast<minutes>(dur_from_epoch)}; // {2} 
    std::cout << "Duration in nanoseconds: " << dur_from_epoch << '\n'; //{3} 
    std::cout << "Duration in minutes: " << dur_minutes << '\n'; //{4}
}

int main() {
    duration_cast();
    return 0;
}