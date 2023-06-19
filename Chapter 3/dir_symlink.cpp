#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    const path path_to_iterate{"test_dir"};
    for (auto const& dir_entry : 
        recursive_directory_iterator{path_to_iterate}) {
        auto result = is_symlink(dir_entry.path());
        if (result) cout << read_symlink(dir_entry.path());
    }
    return 0;
}