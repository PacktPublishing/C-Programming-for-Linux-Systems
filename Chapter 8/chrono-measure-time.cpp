#include <iostream>
#include <chrono>
#include <algorithm>

static const auto LIMIT{10000};
void just_busy_wait_f() {
    for (auto i{0}; i < LIMIT; ++i) {
        for (auto j{0}; j < LIMIT; ++j);
    }
}

using namespace std::chrono;

void first_duration() {
    const auto start{steady_clock::now()}; // {1}
    just_busy_wait_f(); // {2}
    const auto end{steady_clock::now()}; // {3}
    const auto duration{duration_cast<milliseconds>(end - start)}; // {4}
    std::cout << "Execution time: " << duration.count() << " milliseconds\n"; // {5}
}

int main() {
    first_duration();
    return 0;
}