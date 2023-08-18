int main() {
    try {
        const int fd{open("/tmp/cpp-test-file", O_RDONLY)}; // {1}
        if (fd == -1) { return errno; }
        // Do something with the file and suddenly something throws {2}
        if (close(fd) == -1) { return errno; } // {3}
    } catch (...) {
        std::cerr << "Something somewhere went terribly wrong!\n";
        return -1;
    }
    return 0;
}