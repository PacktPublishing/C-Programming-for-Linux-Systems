#include <condition_variable>
#include <iostream>
#include <memory>
#include <mutex>
#include <thread>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <sys/mman.h>

using namespace std;

constexpr auto PROT_RW = PROT_READ | PROT_WRITE;
constexpr auto SHM_ID  = "/test_shm";

string_view message = "This is a testing message!";

mutex _mutex;
condition_variable cond_var;
bool work_done = false;

template<typename T>
struct mmap_deallocator {
    size_t m_size;
    mmap_deallocator(size_t size) : m_size{size} {}

    void operator()(T *ptr) const {
       munmap(ptr, m_size);
    }
};

template<typename T, typename... Args>
auto producer(Args&&... args) {
    unique_lock<mutex> lock(_mutex);
    cond_var.wait(lock, []{ return work_done == false; });
    if (int fd = shm_open(SHM_ID, O_CREAT | O_RDWR, 0644); 
       fd != -1) {
        ftruncate(fd, sizeof(T));
        if (auto ptr = mmap(0, sizeof(T), 
                            PROT_RW, MAP_SHARED, 
                            fd, 0)) {
            auto obj = new (ptr) T(args...);
            auto del = mmap_deallocator<T>(sizeof(T));
            work_done = true;
            cond_var.notify_one();
            return unique_ptr<T, 
                mmap_deallocator<T>>(obj, del);
        }
    }
    throw bad_alloc();
}

template<typename T>
auto consumer() {
    unique_lock<mutex> lock(_mutex);
    cond_var.wait(lock, []{ return work_done == true; });
    if (int fd = shm_open(SHM_ID, O_RDWR, 0644); 
       fd != -1) {
        ftruncate(fd, sizeof(T));
        if (auto ptr = mmap(0, sizeof(T), 
                            PROT_RW, MAP_SHARED, 
                            fd, 0)) {
            auto obj = static_cast<T*>(ptr);
            auto del = mmap_deallocator<T>(sizeof(T));
            work_done = false;
            return unique_ptr<T, 
                mmap_deallocator<T>>(obj, del);
        }
    }
    throw bad_alloc();
}

int main() {
    thread server([]{ 
        auto ptr = 
            producer<string_view>(message);
            cout << "Sending: " << *ptr << '\n';
    });
    thread client([]{ 
        auto ptr = 
            consumer<string_view>();
            cout << "Receiving: " << *ptr<< '\n'; 
    });

    server.join();
    client.join();
    return 0;
}