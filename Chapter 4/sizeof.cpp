#include <iostream>

struct Empty {};
struct Padding {
    long test;
    char m;
};
struct Virt {
    virtual char GetChar() const { return ch; }
    char ch;
};

void SizeOfStructs() {
    std::cout << "Empty: " << sizeof(Empty) << '\n';
    std::cout << "Padding: " << sizeof(Padding) << '\n';
    std::cout << "Virt: " << sizeof(Virt) << '\n';
}

void SizeOfPrimitives() {
    int i;
    long l;
    char* p;
    std::cout << "sizeof(int) = " << sizeof(int) << "; sizeof(i) = " << sizeof(i) << '\n';
    std::cout << "sizeof(long) = " << sizeof(long) << "; sizeof(l) = " << sizeof(l) << '\n';
    std::cout << "sizeof(char*) = " << sizeof(char*) << "; sizeof(p) = " << sizeof(p) << '\n';
}

void SizeOfRefs() {
    char c;
    char& r_c{c};
    char* p_c;
    std::cout << "sizeof(char) = " << sizeof(char) << "; sizeof(c) = "     << sizeof(c) << '\n';
    std::cout << "sizeof(char&) = " << sizeof(char&) << "; sizeof(r_c) = " << sizeof(r_c) << '\n';
    std::cout << "sizeof(char*) = " << sizeof(char*) << "; sizeof(p_c) = " << sizeof(p_c) << '\n';
}

int main() {
    SizeOfPrimitives();
    SizeOfStructs();
    SizeOfRefs();
    return 0;
}