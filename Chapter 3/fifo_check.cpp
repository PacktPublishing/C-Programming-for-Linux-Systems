#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    if (exists("example_fifo") && is_fifo("example_fifo")) {
        remove("example_fifo");
        cout << "FIFO is removed";
    }

    return 0;
}