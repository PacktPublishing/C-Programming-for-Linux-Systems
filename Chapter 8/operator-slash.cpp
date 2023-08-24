#include <iostream>
#include <chrono>

using namespace std::chrono;

void operator_slash() {
    year_month_day date1{July/5d/2023y};
    year_month_day date2{1d/October/2023y};
    year_month_day date3{2023y/January/27d};
    std::cout << date1 << '\n';
    std::cout << date2 << '\n';
    std::cout << date3 << '\n';
}

int main() {
    operator_slash();
    return 0;
}