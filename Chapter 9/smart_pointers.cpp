#include <iostream>
#include <memory>

using namespace std;

struct Book {
    string_view title;
    Book(string_view p_title) : title(p_title) {
        cout << "Constructor for: " << title << endl;
    }

    ~Book() {
        cout << "Destructor for: " << title << endl;
    }
};

int main() {
    unique_ptr<Book> book1 = 
        make_unique<Book>("Jaws");
    unique_ptr<Book> book1_new;
    book1_new = move(book1);
    cout << book1_new->title << endl;

    shared_ptr<Book> book2 = 
        make_unique<Book>("Dune");
    shared_ptr<Book> book2_new;
    book2_new = book2;

    cout << book2->title << " "
         << book2_new->title 
         << endl;
    cout << book2.use_count() << endl;

    return 0;
}