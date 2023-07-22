#include <iostream>
#include <memory>

using namespace std;
using std::shared_ptr;
using std::weak_ptr;

struct Book {
    string_view title;
};

struct ListNode {
    Book data;
    ListNode(string_view p_title) {
        data.title = p_title;
        cout << "Node created: " << data.title
             << endl;
    }
    ~ListNode() {
        cout << "Node destroyed: " << data.title
             << endl;
    }
    shared_ptr<ListNode> next;
    weak_ptr<ListNode> prev;
};

int main() {
    shared_ptr<ListNode> head{new ListNode{"Dune"}};
    head->next = shared_ptr<ListNode>{new ListNode{"Jaws"}};
    head->next->prev = head;
    return 0;
}