#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

int main() {
    const int fd{open("no-such-file.txt", O_RDONLY)}; // {1}
    if (fd == -1) {
        std::cerr << "Error opening file: " << strerror(errno) << '\n';
        std::cerr << "Error code: " << errno << '\n';
        return EXIT_FAILURE;
    }
    // Do something with the file...
    if (close(fd) == -1) {
        std::cerr << "Error closing file: " << strerror(errno) << '\n';
        std::cerr << "Error code: " << errno << '\n';
        return EXIT_FAILURE;
    }
    return 0;
}