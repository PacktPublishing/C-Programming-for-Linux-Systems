#include <iostream>
#include <sys/stat.h>

void CreateDirectory(const std::string& dirPath) {
    using namespace std;
    if (mkdir(dirPath.c_str(), 0777) != 0) {
        const auto ecode{make_error_code(errc{errno})}; // {1}
        cout << "CreateDirectory reports error: " << ecode.message() << '\n';
        throw system_error{ecode}; // {2}
    }
}

int main() {
    try {
        CreateDirectory("/tmp/test"); // First try
        CreateDirectory("/tmp/test"); // Second try throws
    } catch (const std::system_error& se) { // {3}
        const auto econd{se.code().default_error_condition()}; // {4}
        if (econd != std::errc::file_exists) { // {5}
            std::cerr << "Unexpected system error: " << se.what() << '\n';
            throw; // {6}
        }
        std::cout << "Nothing unexpected, safe to continue.\n";
    }

    return 0;
}