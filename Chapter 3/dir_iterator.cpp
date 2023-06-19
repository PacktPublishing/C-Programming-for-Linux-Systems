#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    const path path_to_iterate{"test_dir"};
    for (auto const& dir_entry : 
        directory_iterator{path_to_iterate}) {
        cout << dir_entry.path() << endl;
    }

    cout << endl;

    for (auto const& dir_entry : 
        recursive_directory_iterator{path_to_iterate}) {
        cout << dir_entry.path() << endl;
    }
    return 0;
}