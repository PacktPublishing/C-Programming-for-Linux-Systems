#include <iostream>
#include <chrono>

using namespace std::chrono;

void get_date_time() {
    year this_year{2023};
    month this_month{8};
    day this_day{4};
    std::cout << "Year: " << this_year ;
    std::cout << ", Month: " << this_month; 
    std::cout << ", Day: " << this_day << '\n';
}

int main() {
    get_date_time();
    return 0;
}