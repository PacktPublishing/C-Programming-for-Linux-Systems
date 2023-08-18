#include <iostream>
#include <sys/stat.h>

std::error_code CreateDirectory(const std::string& dirPath) {
    std::error_code ecode{};
    if (mkdir(dirPath.c_str(), 0777) != 0) {
        ecode = std::error_code{errno, std::generic_category()}; // {1}
    }

    return ecode;
}

int main() {
    auto ecode{CreateDirectory("/tmp/test")};
    if (ecode){ // {2}
        std::cerr << "Error 1: " << ecode.message() << '\n';
    }

    ecode = CreateDirectory("/tmp/test"); // {3}
    if (ecode){
        std::cerr << "Error 2: " << ecode.message() << '\n';
    }

    if (ecode.value() == EEXIST) { // {4}
        std::cout << "This is platform specific and not portable.\n";
    }
    return 0;
}