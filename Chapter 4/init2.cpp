#include <string>

class Person {
public:
    explicit Person(const std::string& the_name) : name{the_name} {}
private:
    std::string name;
};
class Employee {
public:
    Employee(const Person& p) : p{p} {}
    explicit Employee(const Employee& e) : p{e.p} {}
private:
    Person p;
};
void init2() {
    int c{2};
    double mm{0.0};
    Person john{"John M."};

    int d(c);     // {1} Copy initialization
    int e{d};     // {2} Copy list initialization
    int f = e;    // {3} Copy initialization
    int f1 = {d}; // {4} Copy list initialization

    Employee staff1{john}; // OK
    // Employee staff2{std::string{"George"}}; // Error - explicit
    Employee staff3{staff1}; // OK
    // Employee staff4 = staff1; // Error - explicit copy ctor
    // Employee staff5 = {staff1}; // Error - explicit copy ctor
}

int main() {
    init2();
    return 0;
}