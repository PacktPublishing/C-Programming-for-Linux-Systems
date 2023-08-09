#include <array>
#include <iostream>
#include <string_view>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

constexpr auto PORT     = 8080;
constexpr auto BUF_SIZE = 16;

int main() {
    constexpr auto ip       = "192.168.136.128";
    struct sockaddr_in addr = { 0 };
    socklen_t addr_size     = 0;

    if (auto serv_sock = 
            socket(AF_INET, SOCK_STREAM, 0);
            serv_sock < 0) {
        const auto ecode
            { make_error_code(errc{errno}) };
        cerr << "Error opening socket!";
        system_error exception{ ecode };
        throw exception;
    }
    else {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(PORT);
        addr.sin_addr.s_addr = inet_addr(ip);

        if (auto res =
                connect(serv_sock,
                        (struct sockaddr*)&addr,
                        sizeof(addr)); res == -1) {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error connecting to socket!";
            system_error exception{ ecode };
            throw exception;
        }

        string_view req = { to_string(getpid()) };
        cout << "Client request: " << req << endl;
        if (auto res =
                send(serv_sock, 
                     req.data(), 
                     req.size(),
                     MSG_DONTWAIT);
                res >= 0) {
            array<char, BUF_SIZE> buffer{};
            if (auto b_recv =
                    recv(serv_sock, 
                         buffer.data(), 
                         buffer.size(), 
                         MSG_PEEK);
                    res > 0) {
                buffer.data()[b_recv] = '\0';
                cout << "Server response: " 
                     << buffer.data();
            }
            else {
                cerr << "Error sending to socket!";
            }
        }
        else {
            cerr << "Error receiving from socket!";
        }
        if (auto res =
                close(serv_sock); res == -1) {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error closing socket!";
            system_error exception{ ecode };
            throw exception;
        }
        cout << "\nJob done! Disconnecting." << endl;
    }
    return 0;
}