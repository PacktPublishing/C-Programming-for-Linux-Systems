#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <array>
#include <optional>
#include <coroutine>
#include <iostream>
#include <memory>
#include <string_view>
#include <thread>

using namespace std;

constexpr size_t BUF_SIZE = 256;
constexpr auto   PORT = 8080;

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
            currValue = value;
            return suspend_always{};
        }

        void unhandled_exception() {
            exit(EXIT_FAILURE);                    // {2}
        }

        T currValue;
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
    Generator(const Generator&) = delete;
    Generator& operator = (const Generator&) = delete;
    Generator(Generator&& other) noexcept :
        c_routine(other.c_routine) {
        other.c_routine = {};
    }
    Generator& operator = (Generator&& other) noexcept {
        if (this == &other)
            return *this;
        if (c_routine)
            c_routine.destroy();
        c_routine = other.c_routine;
        other.c_routine = {};
        return *this;
    }

    optional<T> operator()() {
        c_routine.resume();
        if (c_routine.done()) {
            return nullopt;
        }
        return c_routine.promise().currValue;
    }
};

Generator<size_t> send_to(int sockfd,
    string_view buffer,
    auto servaddr) noexcept {
    for (;;) {
        auto value = sendto(sockfd,
                            buffer.data(),
                            buffer.size(),
                            MSG_DONTWAIT,
                            (const struct sockaddr*)
                            &servaddr,
            sizeof(servaddr));          // {6}

        co_yield value;                                 // {7}
    }
}

Generator<string> recv_from(int sockfd,
                            auto clntaddr,
                            size_t buf_size = BUF_SIZE)
                            noexcept {
    socklen_t len = sizeof(struct sockaddr_in);
    array<char, BUF_SIZE> tmp_buf = {};
    for (;;) {
        recvfrom(sockfd,
                 tmp_buf.data(),
                 tmp_buf.size(),
                 MSG_DONTWAIT,
                 (struct sockaddr*)&clntaddr,
                 &len);
        co_yield tmp_buf.data(); // {8}
    }
}

int main() {
    auto sockfd = 0;

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        const auto ecode{ make_error_code(errc{errno}) };
        cerr << "Error opening shm region";
        system_error exception{ ecode };
        throw exception;
    }

    auto server = jthread([&sockfd] {
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
            auto sentData = sender();
            cout << i << " Bytes sent: "
                << *sentData << endl;     // {10}
        }
        });

    auto client = jthread([&sockfd] {
        cout << "\nrecv_from():\n" << endl;
        struct sockaddr_in clntaddr = { 0 };
        auto receiver = recv_from(sockfd, clntaddr);
        for (auto i = 1; i <= 10; i++) {
            auto recvData = receiver();
            cout << i << " Message received: "
                << *recvData << endl;   // {11}
        }
        });

    server.join();
    client.join();
    close(sockfd);
    return 0;
}