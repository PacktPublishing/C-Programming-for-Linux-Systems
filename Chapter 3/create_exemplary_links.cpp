#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    const path path_to_iterate{"test_dir"};
    if (exists("some_data")) {
        create_hard_link("some_data", "some_data_hl");
        create_symlink("some_data", "some_data_sl");
    }
    return 0;
}