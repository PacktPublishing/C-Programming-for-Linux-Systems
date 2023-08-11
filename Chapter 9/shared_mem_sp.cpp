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

constexpr auto PROT_RW  = PROT_READ | PROT_WRITE;
constexpr auto SHM_ID   = "/test_shm";
constexpr auto BUF_SIZE = 256;

string_view message{ "This is a testing message!" };

mutex cv_mutex;
condition_variable cond_var;
bool work_done = false;

template<typename T>
struct mmap_deallocator {
    size_t m_size;
    mmap_deallocator(size_t size) : m_size{ size } {}

    void operator()(T* ptr) const {
        munmap(ptr, m_size);
    }
};

template<typename T, typename N>
auto producer(T buffer, N size) {
    unique_lock<mutex> lock(cv_mutex);
    cond_var.wait(lock, [] { return work_done == false; });
    if (int fd = 
            shm_open(SHM_ID, O_CREAT | O_RDWR, 0644);
                     fd != -1) {
        ftruncate(fd, size);
        if (auto ptr = 
                mmap(0, size,
                     PROT_RW, MAP_SHARED,
                     fd, 0); ptr != MAP_FAILED) {
            auto obj = new (ptr) T(buffer);
            auto del = mmap_deallocator<T>(size);
            work_done = true;
            lock.unlock();
            cond_var.notify_one();
            return unique_ptr<T,
                mmap_deallocator<T>>(obj, del);
        }
        else {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error mapping the shmem!";
            system_error exception{ ecode };
            throw exception;
        }
    }
    else {
        const auto ecode
            { make_error_code(errc{errno}) };
        cerr << "Error opening the shmem!";
        system_error exception{ ecode };
        throw exception;
    }
    // Some shm function failed.
    throw bad_alloc();
}

template<typename T, typename N>
auto consumer(N size) {
    unique_lock<mutex> lock(cv_mutex);
    cond_var.wait(lock, [] { return work_done == true; });
    if (int fd = 
            shm_open(SHM_ID, O_RDWR, 0644); fd != -1) {
        ftruncate(fd, size);
        if (auto ptr = 
                mmap(0, size, PROT_RW, MAP_SHARED,
                     fd, 0); ptr != MAP_FAILED) {
            auto obj = static_cast<T*>(ptr);
            auto del = mmap_deallocator<T>(sizeof(T));
            work_done = false;
            return unique_ptr<T,
                mmap_deallocator<T>>(obj, del);
        }
        else {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error mapping the shmem!";
            system_error exception{ ecode };
            throw exception;
        }
    }
    else {
        const auto ecode
            { make_error_code(errc{errno}) };
        cerr << "Error opening the shmem!";
        system_error exception{ ecode };
        throw exception;
    }
    // Some shm function failed.
    throw bad_alloc();
}

int main() {

    thread server([] {
        auto ptr =
            producer<const char*, size_t>(message.data(), message.size());
        cout << "Sending: " << *ptr << '\n';
        });
    thread client([] {
        auto ptr =
            consumer<char*, size_t>(message.size());
        cout << "Receiving: " << *ptr << '\n';
        });

    server.join();
    client.join();
    return 0;
}