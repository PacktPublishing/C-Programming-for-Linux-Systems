#include <iostream>

void Throw() {
    using namespace std;
    throw system_error{make_error_code(errc::bad_file_descriptor)};
}

int main() {
    using namespace std;
    try {
        try {
            Throw(); // {1}
        } catch (runtime_error e) { // {2}
            throw e; // {3}
        }
    } catch (const exception& e) { // {4}
        const system_error& se{dynamic_cast<const system_error&>(e)}; // {5}
        const auto econd{se.code().default_error_condition()};
        std::cerr << econd.message() << '\n';
    }
    return 0;
}