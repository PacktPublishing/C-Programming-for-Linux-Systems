#include <iostream> 

int global_var = 1; // Static storage duration 

void foo() { 
    int automatic_var = 2; 
    static int static_var = 3; 
    int* dynamic_var = new int(4); 
    std::cout << "Automatic var: " << automatic_var << '\n'; 
    std::cout << "Static var: " << static_var << '\n'; 
    std::cout << "Dynamic var: " << *dynamic_var << '\n';   
    delete dynamic_var; 
} 

int main() { 
    foo(); 
    std::cout << "Global var: " << global_var << '\n';  
    return 0; 
} 