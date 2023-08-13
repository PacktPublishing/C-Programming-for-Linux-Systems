#include <iostream>
#include <vector>
#include <algorithm>
#include <cmath>
#include <source_location>

struct Mean {
    Mean() noexcept   {
        std::cout << 
            std::source_location::current().function_name() << 
            " of " << this << '\n';
    }
    Mean(Mean&& a) noexcept : sum{a.sum}, count{a.count} {
        std::cout << 
            std::source_location::current().function_name() << 
            " from: " << &a << " to: " << this << '\n';
        a.sum = 0;
        a.count = -1;
    }
    Mean& operator=(Mean&& a) noexcept {
        std::cout << 
            std::source_location::current().function_name() << 
            " from: " << &a << " to: " << this << '\n';
        sum = a.sum;
        count = a.count;  
        return *this;
    }
    Mean(const Mean& a) noexcept : sum{a.sum}, count{a.count} {
        std::cout << 
            std::source_location::current().function_name() << 
            " from: " << &a << " to: " << this << '\n';
    }
    ~Mean() noexcept {
        std::cout << 
            std::source_location::current().function_name() << 
            " of " << this << '\n';
    }
    void operator()(const double& val) {
        std::cout << 
            std::source_location::current().function_name() << 
            " of " << this << '\n';
        sum += val;
        ++count;
    }
private:
    double sum{};
    int count{};
    friend std::ostream& operator<<(std::ostream& os, const Mean& a);
};

std::ostream& operator<<(std::ostream& os, const Mean& a) {
    double mean{std::nan("")};
    if (a.count > 0) {
        mean = a.sum / a.count;
    }
    os << mean;
    return os;
}

int main() {
    Mean calc_mean;
    std::vector v1{1.0, 2.5, 4.0, 5.5};
    std::cout << "Start calculation\n";
    
    // Pass by value
    //calc_mean = std::for_each(v1.begin(), v1.end(), calc_mean);

    // Pass by reference
    std::for_each(v1.begin(), v1.end(), std::ref(calc_mean));
    
    std::cout << "Finish calculation\n";
    std::cout << calc_mean << '\n';
    return 0;
}