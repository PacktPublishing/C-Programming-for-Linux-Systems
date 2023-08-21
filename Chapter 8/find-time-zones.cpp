#include <iostream>
#include <chrono>

using namespace std::chrono;

void find_time_zones() {
    const tzdb& tzdb{get_tzdb()};
    const std::vector<time_zone>& tzs{tzdb.zones};
    const auto& res{std::find_if(tzs.begin(), tzs.end(), [](const time_zone& tz){
        std::string name{tz.name()};
        return name.ends_with("Sofia");
    })};

    if (res != tzs.end()) {
        try {
            const std::string_view myLocation{res->name()};
            const std::string_view london{"Europe/London"};
            const time_point now{system_clock::now()};
            const zoned_time zt_1{myLocation, now};
            const zoned_time zt_2{london, now};
            std::cout << myLocation << ": " << zt_1 << '\n';
            std::cout << london << ": " << zt_2 << '\n';
        } catch (const std::runtime_error& e) {
            std::cout << e.what() << '\n';
        }
    }
}

int main() {
    find_time_zones();
    return 0;
}