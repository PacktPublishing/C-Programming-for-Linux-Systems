#include <array>
#include <cstring>
#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>

using namespace std;

constexpr auto PORT     = 8072;
constexpr auto BUF_SIZE = 16;

auto die_roll() {
    auto min = 1;
    auto max = 6;
    auto roll = rand() % (max - min + 1) + min;
    return roll;
}

void process_creator() {
    auto sockfd = 0;
    array<char, BUF_SIZE> buffer{};
    string_view stop{ "No more requests!" };
    string_view request{ "Throw dice!" };
    struct sockaddr_in servaddr {};
    struct sockaddr_in cliaddr {};

    servaddr.sin_family = AF_INET; // {1}
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(PORT);

    if (int pid = fork(); pid == 0) {
        // Child
        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) 
                < 0) {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error opening socket!";
            system_error exception{ ecode };
            throw exception;
        } // {2}

        if (bind(sockfd,
            (const struct sockaddr*)&servaddr,
            sizeof(servaddr)) < 0) {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Bind failed!";
            system_error exception{ ecode };
            throw exception;
        } // {3}

        socklen_t len = sizeof(cliaddr);
        for (;;) {
            if (auto bytes_received =
                recvfrom(sockfd, buffer.data(),
                    buffer.size(),
                    MSG_WAITALL,
                    (struct sockaddr*)&cliaddr,
                    &len);
                bytes_received >= 0) { // {4}
                buffer.data()[bytes_received] = '\0';
                cout << "Request received: "
                     << buffer.data() << endl;

                if (strncmp(buffer.data(), 
                            request.data(), 
                            bytes_received) == 0) { // {5}
                    string_view res_data
                        { to_string(die_roll()) }; 
                    sendto(sockfd, res_data.data(),
                           res_data.size(),
                           MSG_WAITALL,
                           (struct sockaddr*)&cliaddr,
                           len);
                }
                else break; // {6}
            }
            else {
                const auto ecode
                    { make_error_code(errc{errno}) };
                cerr << "Receive failed!";
                system_error exception{ ecode };
                throw exception;
            }
        }
        if (auto res = close(sockfd); res == -1) { // {8}
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error closing socket!";
            system_error exception{ ecode };
            throw exception;
        }
        exit(EXIT_SUCCESS);
    }
    else if (pid > 0) {
        // Parent
        int32_t dice_rolls;
        cout << "Chose a number of dice throws"
            << " between 1 and 256." << endl;
        cin >> dice_rolls;
        if (dice_rolls <= 0 and dice_rolls > 256) { // {9}
            dice_rolls = 0;
        }

        if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) 
                < 0) {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error opening socket!";
            system_error exception{ ecode };
            throw exception;
        } // {10}

        socklen_t len = 0;
        for (auto i = 1; i <= dice_rolls; i++) { // {11}    
            if (auto b_sent = sendto(sockfd,
                request.data(),
                request.size(),
                MSG_DONTWAIT,
                (const struct sockaddr*)&servaddr,
                sizeof(servaddr)); b_sent >= 0) { // {12}
            }
            else {
                const auto ecode
                    { make_error_code(errc{errno}) };
                cerr << "Send request failed!";
                system_error exception{ ecode };
                throw exception;
            }
            if (auto b_recv =
                    recvfrom(sockfd,
                             buffer.data(),
                             buffer.size(),
                             MSG_WAITALL,
                             (struct sockaddr*)&servaddr,
                             &len); b_recv > 0) { // {13}
                buffer.data()[b_recv] = '\0';
                cout << "Dice roll result for throw number "
                     << i << " is "
                     << buffer.data() << endl;
            }
        }
        sendto(sockfd,
               stop.data(),
               stop.size(),
               MSG_CONFIRM,
               (const struct sockaddr*)&servaddr,
               sizeof(servaddr)); // {14}

        if (auto res = close(sockfd); res == -1) {
            const auto ecode
                { make_error_code(errc{errno}) };
            cerr << "Error closing socket!";
            system_error exception{ ecode };
            throw exception; // {15}
        }
        exit(EXIT_SUCCESS);
    }
    else {
        const auto ecode
            { make_error_code(errc{errno}) };
        cerr << "Process creation failed!";
        system_error exception{ ecode };
        throw exception; // {16}
    }
}

int main() {
    process_creator();
    return 0;
}
