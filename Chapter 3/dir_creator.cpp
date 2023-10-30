#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    error_code ec;
    auto result = create_directory("test_dir", ec);
    if (result)
        cout << "Directory created successfuly!\n";
    else {
        cout << "Directory creation failed!\n";
        cout << ec.category().name() << endl;
    }

    return 0;
}