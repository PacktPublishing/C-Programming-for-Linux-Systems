#include <algorithm>
#include <iostream>
#include <ranges>
#include <shared_mutex>
#include <string_view>
#include <thread>
#include <vector>

using namespace std;

struct Book {
    string_view title;
    string_view author;
    uint32_t    year;
};

shared_mutex shresMutex;

vector<Book> shared_data = {{"Harry Potter", "Rowling", 1997}, 
                            {"East of Eden", "Steinbeck", 1952}, 
                            {"It", "King", 1986}};

void rd_applyYearFilter(uint32_t yearKey) {
    auto year_filter = 
        [yearKey](Book book) { return book.year < yearKey; };
    shared_lock<shared_mutex> readerLock(shresMutex);
    cout << "Apply year filter: " << endl;
    for (const auto &book : shared_data | 
                            views::filter(year_filter))
        cout << book.title << endl;
}

void rd_Reversed() {
    shared_lock<shared_mutex> readerLock(shresMutex);
    cout << "Apply reversed order: " << endl;
    for (const auto &book : views::reverse(shared_data))
        cout << book.title << endl;
}

void rd_ReversedFilteredByAuthorNameSize(uint32_t nameSizeKey) {
    shared_lock<shared_mutex> readerLock(shresMutex);
    cout << "Apply reversed order and filter by author name size: " << endl;
    for (const auto &book : views::reverse(shared_data) | 
                            views::filter([nameSizeKey](Book book) 
                                { return book.author.size() < nameSizeKey; }))
        cout << book.title << endl;
}

void rd_dropFirstN(uint32_t dropKey) {
    shared_lock<shared_mutex> readerLock(shresMutex);
    cout << "Drop first N elements: " << endl;
    for (const auto &book : views::drop(shared_data, dropKey))
        cout << book.title << endl;
}

void wr_addNewBook(string_view title, 
                   string_view author, 
                   uint32_t year) {
    lock_guard<shared_mutex> writerLock(shresMutex);
    cout << "Add new book: " << title << endl;
    shared_data.emplace_back(Book {title, author, year});
    this_thread::sleep_for(chrono::milliseconds(500));
}

int main() {
    thread yearFilter1(
        []{ rd_applyYearFilter(1990); });
    thread reversed(
        []{ rd_Reversed(); });
    thread reversed_and_filtered(
        []{ rd_ReversedFilteredByAuthorNameSize(8); });
    thread addBook1(
        []{ wr_addNewBook("Dune", "Herbert", 1965); });
    thread dropFirstElements(
        []{ rd_dropFirstN(1); });
    thread addBook2(
        []{ wr_addNewBook("Jaws", "Benchley", 1974); });
    thread yearFilter2(
        []{ rd_applyYearFilter(1970); });
    
    yearFilter1.join();
    reversed.join();
    reversed_and_filtered.join();
    dropFirstElements.join();
    yearFilter2.join();
    addBook1.join();
    addBook2.join();

    cout << "Print subranged books in main thread:" << endl;
    auto const sub_res = 
        ranges::subrange(shared_data.begin()+1, 
                         shared_data.begin()+3);
    for (const auto& book: sub_res){
        cout << book.title << " " << book.author 
             <<  " " << book.year << endl;
    }
    return 0;
}