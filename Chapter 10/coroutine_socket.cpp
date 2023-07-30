#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <coroutine>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>

using namespace std;

constexpr size_t BUF_SIZE = 256;
constexpr auto   PORT     = 8080;

template<typename T>
struct Generator {
    struct promise_type {
        promise_type() = default;
        ~promise_type() = default;

        auto get_return_object() {
            return Generator{
                handle_type::from_promise(*this)
            };
        }
        auto initial_suspend() {
            return suspend_always{};
        }
        auto final_suspend() noexcept {
            return suspend_always{};
        }

        auto return_void() {
            return suspend_never{};
        }

        auto yield_value(const T value) {          // {1}
            current_value = value;
            return suspend_always{};
        }

        void unhandled_exception() {
            exit(EXIT_FAILURE);                    // {2}
        }

        T current_value;
    };

private:
    using handle_type = coroutine_handle<promise_type>;
    handle_type c_routine;

public:
    explicit Generator(handle_type handle) :
        c_routine(handle) {}                             // {3}

    ~Generator() {
        if (c_routine) c_routine.destroy();              // {4}
    }
    Generator(const Generator&)              = delete;
    Generator& operator = (const Generator&) = delete;
    Generator(Generator&& other) noexcept : 
        c_routine(other.c_routine) {
        other.c_routine = nullptr;
    }
    Generator& operator = (Generator&& other) noexcept {
        c_routine = other.c_routine;
        other.c_routine = nullptr;
        return *this;
    }

    T getCurrentValue() {
        return c_routine.promise().current_value;
    }

    bool proceed() {                                             
        c_routine.resume();
        return not c_routine.done();                    // {5}
    }
};

Generator<size_t> send_to(int sockfd, 
                          string_view buffer, 
                          auto servaddr) noexcept {
    for (;;) {
        auto value = sendto(sockfd,
                            buffer.data(),
                            buffer.size(),
                            0,
                            (const struct sockaddr*)
                                &servaddr,
                            sizeof(servaddr));          // {6}

        co_yield value;                                 // {7}
    }
}

Generator<string_view> recv_from(int sockfd, 
                                 auto clntaddr, 
                                 size_t buf_size = BUF_SIZE) 
                                    noexcept {
    socklen_t len = sizeof(struct sockaddr_in);
    char tmp_buf[BUF_SIZE] = { 0 };
    for (;;) {
         recvfrom(sockfd,
                  (char *)tmp_buf,
                  buf_size,
                  MSG_DONTWAIT,
                  (struct sockaddr*)&clntaddr,
                  &len);                                

        auto value{ tmp_buf };
        co_yield value;                                // {8}
    }
}

int main() {
    auto sockfd = 0;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    auto server = thread([&sockfd] {
        struct sockaddr_in servaddr = { 0 };
        servaddr.sin_family = AF_INET;
        servaddr.sin_addr.s_addr = INADDR_ANY;
        servaddr.sin_port = htons(PORT);

        if (bind(sockfd,
            (const struct sockaddr*)&servaddr,
            sizeof(struct sockaddr_in)) < 0) {
            perror("Bind failed");
            exit(EXIT_FAILURE);
        }

        cout << "\nsend_to():\n";
        string_view message{ "This is a test!" };
        auto sender = send_to(sockfd, message, servaddr); // {9}
        for (int i = 1; i <= 10; i++) {
            sender.proceed();
            cout << i << " Bytes sent: " 
                 << sender.getCurrentValue() << endl;     // {10}
        }
    });

    auto client = thread([&sockfd] {
        cout << "\nrecv_from():\n" << endl;
        struct sockaddr_in clntaddr = { 0 };
        auto receiver = recv_from(sockfd, clntaddr);
        for (int i = 1; i <= 10; i++) {
            receiver.proceed();
            cout << i << " Message received: " 
                 << receiver.getCurrentValue() << endl;   // {11}
        }
    });

    server.join();
    client.join();
    close(sockfd);
    return 0;
}