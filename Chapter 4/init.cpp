#include <iostream>

struct Point {
    double x;
    double y;
};

struct Person {
    std::string name;
    int age;  
};

void init1() {
    long a; // {1}
    Point p1; // {2}
    std::cout << "{1}: " << a << '\n';
    std::cout << "{2}: " << p1.x << ", " << p1.y << '\n';
}

void init2() {
    int b(1);         // {3.1}
    int b_trunc(1.2); // {3.2}
    int c{2};         // {4.1}
    // int c_warn{2.2};  // {4.2} Direct list initialization => narrowing conversion error

    std::cout << "{3.1}: " << b << '\n';
    std::cout << "{3.2}: " << b_trunc << '\n';
    std::cout << "{4.1}: " << c << '\n';
}

void init3() {
    int zero1{}; // {1}
    int zero2 = int(); // {2}
    int zero3 = int{}; // {3}
    Person nick{"Nick L.", 42}; // {4}
    Person john{.name{"John M."}, .age{24}}; // {5}
}

void init4() {
    int c{2};

    int d(c); // {1}
    int e{d}; // {2}
    int f = e; // {3}
    int f1 = {d}; // {4}
}

int main() {
    init1();
    init2();
    init3();
    init4();
    return 0;
}