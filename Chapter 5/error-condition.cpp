#include <iostream>
#include <sys/stat.h>


std::error_code CreateDirectory(const std::string& dirPath) {
    std::error_code ecode{};
    if (mkdir(dirPath.c_str(), 0777) != 0) {
        std::errc cond{errno}; // {1}
        ecode = std::make_error_code(cond); // {2}
    }

    return ecode;
}

int main() {
    auto ecode{CreateDirectory("/tmp/test")};
    if (ecode){
        std::cerr << "Error 1: " << ecode.message() << '\n';
    }

    ecode = CreateDirectory("/tmp/test");
    if (ecode){
        std::cerr << "Error 2: " << ecode.message() << '\n';
    }

    if (ecode == std::errc::file_exists) { // {3}
        std::cout << "This is platform agnostic and is portable.\n";
    }
    return 0;
}