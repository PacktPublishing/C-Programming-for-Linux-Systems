#include <iostream>
#include <chrono>

using namespace std::chrono;

void leap_year(){
    sys_time now{system_clock::now()};
    year_month_day today{floor<days>(now)};
    std::cout << "Today is: " << today << '\n';
    year thisYear{today.year()};
    std::cout << "Year " << thisYear;
    if (thisYear.is_leap()) {
        std::cout << " is a leap year\n";
    } else {
        std::cout << " is not a leap year\n";
    }
}

int main() {
    leap_year();
    return 0;
}