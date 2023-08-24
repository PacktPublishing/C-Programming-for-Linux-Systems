#include <barrier>
#include <iostream>
#include <vector>
#include <thread>
#include <tuple>
#include <random>
#include <algorithm>

using Temperature = 
    std::tuple<std::string, // The name of the room
               std::vector<double>, // Temperature measurements
               double>; // Calculated mean temperature 
                        // value for a specific room
std::vector<Temperature> room_temperatures {
    {"living_room",{}, 0.0}, 
    {"bedroom", {}, 0.0},
    {"kitchen", {}, 0.0},
    {"closet", {}, 0.0}
};

void GetTemperatures(Temperature& temp) {
    std::mt19937 gen{std::random_device{}()};

    // Add normal distribution with mean = 20
    // and standard deviation of 8
    std::normal_distribution<> d{20, 8};

    auto& input_data{std::get<1>(temp)};
    input_data.clear();
    for (auto n{0}; n < 10; ++n) {
        // Add input data
        input_data.emplace_back(d(gen));
    }
}

void ProcessMeasurement(Temperature& temp){
    const auto& values{std::get<1>(temp)};
    auto& mean{std::get<2>(temp)};
    mean = std::reduce(values.begin(), values.end()) / values.size();
}

int main() {
    // Init data
    std::ranges::for_each(room_temperatures, GetTemperatures);

    std::stop_source message;
    std::barrier measurementBarrier{ // {1}
        static_cast<int>(room_temperatures.size()), // {2}
        [&message]() noexcept { // {3}
            // Get all results
            double mean{0.0};
            for (const auto& room_t : room_temperatures) {
                std::cout << "Mean temperature in " 
                          << std::get<0>(room_t) 
                          << " is " << std::get<2>(room_t) << ".\n";
                mean += std::get<2>(room_t);
            }
            mean /= room_temperatures.size();
            std::cout << "Mean temperature in your home is " << mean << " degrees Celsius.\n";
            std::cout << "=============================================\n";

            // Add new input data
            std::ranges::for_each(room_temperatures, GetTemperatures);

            // Make 4 measurements and request stop.
            static unsigned timer{0};
            if (timer >= 3) {
                message.request_stop();
            }
            ++timer;
        }
    };

    std::vector<std::jthread> measurementSensors;
    for (auto& temp : room_temperatures) {
        measurementSensors.emplace_back([&measurementBarrier, &message, &temp](){
            const auto& token = message.get_token();
            while(!token.stop_requested()) {
                ProcessMeasurement(temp);
                measurementBarrier.arrive_and_wait(); // {4}
            }
        });
    }

    return 0;
}