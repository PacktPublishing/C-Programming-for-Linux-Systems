#include <iostream>
#include <chrono>

using namespace std::chrono;

void get_time_zones() {
    const tzdb& tzdb{get_tzdb()}; 
    const std::vector<time_zone>& tzs{tzdb.zones}; 
    for (const time_zone& tz : tzs) { 
        std::cout << tz.name() << '\n';
    } 
}

int main() {
    get_time_zones();
    return 0;
}