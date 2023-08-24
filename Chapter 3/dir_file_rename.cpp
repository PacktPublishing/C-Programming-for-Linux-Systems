#include <iostream>
#include <filesystem>

using namespace std;
using namespace std::filesystem;

int main() {
    if (exists("some_data_sl")) {
        rename("some_data_sl", "some_data_sl_rndm");
    }
    return 0;
}