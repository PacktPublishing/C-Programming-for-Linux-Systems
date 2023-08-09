#include <array>
#include <iostream>
#include <signal.h>
#include <cstdlib>
#include <cstring>
#include <string_view>

#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

constexpr auto PORT     = 8080;
constexpr auto BUF_SIZE = 256;
constexpr auto BACKLOG  = 5;
constexpr auto SIG_MAX  = 128;
void exitHandler(int sig) {
    cerr << "Exit command called - terminating server!" 
         << endl;
    exit(SIG_MAX + sig);
}

int main() {
    signal(SIGINT, exitHandler);

    constexpr auto ip = "192.168.136.128";
    auto client_sock  = 0;

    struct sockaddr_in server_addr = { 0 };
    struct sockaddr_in client_addr = { 0 };

    socklen_t addr_size = 0;
    auto result         = 0;

    if (auto server_sock = 
            socket(AF_INET, SOCK_STREAM, 0); 
            server_sock < 0) {
        const auto ecode
            { make_error_code(errc{errno}) };
        cerr << "Error opening socket!";
        system_error exception{ ecode };
        throw exception;
    }
    else {
        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr(ip);

        result = bind(server_sock,
            (struct sockaddr*)&server_addr,
            sizeof(server_addr));
        if (result < 0) {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error binding socket!";
            system_error exception{ ecode };
            throw exception;
        }

        result = listen(server_sock, BACKLOG);
        if (result != 0) {
            cerr << "Cannot accept connection";
        }

        cout << "Listening..." << endl;

        for (;;) {
            addr_size = sizeof(client_addr);
            client_sock =
                accept(server_sock,
                       (struct sockaddr*)&client_addr,
                       &addr_size);

            if (client_sock > 0) {
                cout << "Client connected." << endl;
                array<char, BUF_SIZE> buffer{};

                if (auto b_recv = recv(client_sock,
                                       buffer.data(),
                                       buffer.size(),
                                       MSG_PEEK); 
                                  b_recv > 0) {
                    buffer.data()[b_recv] = '\0';
                    cout << "Client request: " 
                         << buffer.data() << endl;
                    string_view response = 
                        { to_string(getpid()) };
                    cout << "Server response: "
                         << response << endl;
                    send(client_sock,
                         response.data(),
                         response.size(),
                         MSG_DONTWAIT);
                }
                else {
                    cerr << "Error receiving from socket!";
                }
                if (auto res =
                        close(client_sock); res == -1) {
                    const auto ecode
                        { make_error_code(errc{errno}) };
                    cerr << "Error closing socket!";
                    system_error exception{ ecode };
                    throw exception;
                }
            }
            else {
                cerr << "Client connection failed" << endl;
            }
        }
    }

    return 0;
}