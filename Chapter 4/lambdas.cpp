#include <iostream>
#include <vector>
#include <algorithm>

void mean_lambda() {
   double mean{};
   std::vector v1{1.0, 2.5, 4.0, 5.5};
   std::string_view text{"calculating ..."};
   std::for_each(v1.begin(), v1.end(), 
               [&mean, sum{0.0}, count{0}, text](const double& val) mutable {
       std::cout << text << '\n';
       sum += val;
       ++count;
       mean = sum / count;
    });
    std::cout << mean << '\n';
}

constexpr auto min_lambda = [](const auto& name) -> void {
    std::cout << name << " lambda.\n"; 
};

constexpr auto sum = []<typename T1, typename T2>(T1 a, T2 b) {
    return a*b;
};

class platform_specifi_name {
public:
    template<typename T1, typename T2>
    constexpr auto operator()(const T1& a, const T2& b) const {
        return a*b;
    }
};

int main() {
   min_lambda("Simple");
   mean_lambda();
   constexpr auto val1 = sum(5,3);
   constexpr auto val2 = platform_specifi_name{}(5,3);
   static_assert(val1 == val2);
   return 0;
}