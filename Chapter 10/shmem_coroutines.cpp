#include <atomic>
#include <coroutine>
#include <chrono>
#include <iostream>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <stdexcept>
#include <thread>

#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/mman.h>

constexpr auto PROT_RW = PROT_READ | PROT_WRITE;
constexpr auto SHM_ID = "/test_shm";

using namespace std;

string_view message = "This is a testing message!";

template<typename T>
struct mmap_deallocator {
    size_t m_size;
    mmap_deallocator(size_t size) : m_size{ size } {}

    void operator()(T* ptr) const {
        munmap(ptr, m_size);
    }
};

struct Event {
    Event()                        = default;
    Event(const Event&)            = delete;
    Event(Event&&)                 = delete;
    Event& operator=(const Event&) = delete;
    Event& operator=(Event&&)      = delete;

    class Awaiter;
    Awaiter operator co_await() const noexcept;

    template<typename T, typename N>
    void notify(T, int, N) noexcept;

private:
    mutable atomic<void*> suspended{ nullptr };
    mutable atomic<bool>  notified{ false };

};

struct Event::Awaiter {
    Awaiter(const Event& p_event) : event(p_event) {}

    bool await_ready() const { return false; };
    bool await_suspend(coroutine_handle<> handle) noexcept;
    void await_resume() noexcept {}

private:
    friend class Event;
    const Event& event;
    coroutine_handle<> coroutineHandle;
};

bool 
Event::Awaiter::await_suspend(coroutine_handle<> handle) 
noexcept {
    coroutineHandle = handle;

    if (event.notified) return false;
    event.suspended.store(this);

    return true;
}

Event::Awaiter Event::operator co_await() const noexcept {
    return Awaiter{ *this };
}

struct Task {
    struct promise_type {
        Task get_return_object() { return {}; }
        suspend_never initial_suspend() { return {}; }
        suspend_never final_suspend() noexcept { return {}; }
        void return_void() {}
        void unhandled_exception() {
            exit(EXIT_FAILURE);
        }
    };
};

template<typename T, typename N>
void Event::notify(T buffer, int fd, N size) noexcept {
    notified = false;

    auto* waiter = 
        static_cast<Awaiter*>(suspended.load());
    if (waiter != nullptr) {
        ftruncate(fd, size);
        if (auto ptr = mmap(0, size,
                            PROT_RW, MAP_SHARED,
                            fd, 0)) {
            auto obj = new (ptr) T(buffer);
            auto del = mmap_deallocator<T>(size);
            auto res = 
                unique_ptr<T, mmap_deallocator<T>>(obj, del);
        }
        waiter->coroutineHandle.resume();
    }
}

template<typename T, typename N>
Task receiver(Event& event, int fd, N size) {
    ftruncate(fd, size);
    if (auto ptr = mmap(0, size,
                        PROT_RW, MAP_SHARED,
                        fd, 0)) {
        auto obj = static_cast<T*>(ptr);
        auto del = mmap_deallocator<T>(size);
        auto res = 
            unique_ptr<T, mmap_deallocator<T>>(obj, del);
        cout << *res << endl;
    }
    co_await event;
}

int main() {
    Event event{};
    int fd = shm_open(SHM_ID, O_CREAT | O_RDWR, 0644);
    auto senderT = thread([&event, &fd]{
         event.notify<const char*, size_t>(message.data(), 
                                           fd, 
                                           message.size());
    });

    auto receiverT = thread([&event, &fd]{
         receiver<char*, size_t>(ref(event), 
                                 fd, 
                                 (message.size()));
    });

    senderT.join();
    receiverT.join();
    close(fd);

    return 0;
}