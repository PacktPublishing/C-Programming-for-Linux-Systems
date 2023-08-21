#include <iostream>
#include <chrono>

using namespace std::chrono;

void duration_test() {
    constexpr std::chrono::duration<int, std::ratio<1>> six_minutes_1{360};
    constexpr std::chrono::duration<double, std::ratio<3600>> six_minutes_2{0.1};
    constexpr std::chrono::minutes six_minutes_3{6};
    constexpr auto six_minutes_4{6min};
    std::cout << six_minutes_1 << '\n';
    std::cout << six_minutes_2 << '\n';
    std::cout << six_minutes_3 << '\n';
    std::cout << six_minutes_4 << '\n';
    static_assert(six_minutes_1 == six_minutes_2);
    static_assert(six_minutes_2 == six_minutes_3);
    static_assert(six_minutes_3 == six_minutes_4);
}

int main() {
    duration_test();
    return 0;
}