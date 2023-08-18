#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <fstream>

using namespace std;
class file_guard final {
public:
    file_guard(string_view file, mode_t mode) : // {5}
        fd{open(file.data(), mode)}
    {
        if (fd == -1) {
            throw system_error{make_error_code(errc{errno})};
        }
        cout << "File '" << file << 
        "' with file descriptor '" << 
        fd << "' is opened.\n";
    }
    explicit file_guard(const file_guard&) = delete; // {6}
    file_guard& operator=(const file_guard&) = delete;
    explicit file_guard(file_guard&& other) noexcept : // {7}
        fd{move(other.fd)} { other.fd = -1; }
    file_guard& operator=(file_guard&& other) noexcept
    {
        fd = move(other.fd);
        other.fd = -1;
        return *this;
    }
    int getFileDescriptor() const noexcept { // {8}
        return fd;
    }
    ~file_guard() noexcept { // {9}
        if (fd != -1) {
            close(fd);
            cout << "File with file descriptor '" << fd << "' is closed.\n";   
        }
    }
private:
    int fd;
};
void Throw() {
    cout << "Ops, I need to throw ...\n";
    throw system_error{make_error_code(errc::bad_file_descriptor)};
}
int main() {
    const string_view myFileName{"/tmp/cpp-test-file"}; // {1}
    ofstream theFile(myFileName.data()); // {2}
    try {
        file_guard guard(myFileName, O_RDONLY); // {3}
        const auto fd = guard.getFileDescriptor();
        Throw(); // {4}
    } catch (const exception& e) {
        cout << e.what();
        return -1;
    }
    return 0;
}