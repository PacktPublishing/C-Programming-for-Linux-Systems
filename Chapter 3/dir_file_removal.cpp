#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    if (exists("some_data")) {
        std::filesystem::space_info space_obj = 
            space(current_path());
        cout << "Capacity: " 
            << space_obj.capacity << endl;
        cout << "Free: " 
            << space_obj.free << endl;
        cout << "Available: " 
            << space_obj.available << endl;

        remove("some_data");
        space_obj = space(current_path());

        cout << "Capacity: " 
            << space_obj.capacity << endl;
        cout << "Free: " 
            << space_obj.free << endl;
        cout << "Available: " 
            << space_obj.available << endl;
    }
    return 0;
}