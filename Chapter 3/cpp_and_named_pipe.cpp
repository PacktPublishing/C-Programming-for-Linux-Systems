#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <array>
#include <iostream>
#include <filesystem>
#include <string_view>

using namespace std;
using namespace std::filesystem;

static string_view fifo_name     = "example_fifo"; // {1}
static constexpr size_t buf_size = 64;

void write(int out_fd, 
           string_view message) { // {2}
    write(out_fd,
          message.data(),
          message.size()); 
}

string read(int in_fd) { // {3}
    array <char, buf_size> buffer;
    size_t bytes = read(in_fd,
                        buffer.data(),
                        buffer.size());
    if (bytes > 0) {
        return {buffer.data(), bytes}; // {4}
    }
    return {};
}

int main() {
    if (!exists(fifo_name))
        mkfifo(fifo_name.data(), 0666); // {5}

    if (pid_t childId = fork(); childId == -1) {
        perror("Process creation failed");
        exit(EXIT_FAILURE);
    }
    else {
        if(is_fifo(fifo_name)) { // {6}
            if (childId == 0) {
                if (int named_pipe_fd =
                        open(fifo_name.data(), O_RDWR);
                    named_pipe_fd >= 0) { // {7}
                    string message;
                    message.reserve(buf_size);
                    sleep(1);
                    message = read(named_pipe_fd); // {8}
                    string_view response_msg
                        = "Child printed the message!";
                    cout << "Child: " << message << endl;
                    write(named_pipe_fd,
                          response_msg); // {9}
                    close(named_pipe_fd);
                }
                else {
                    cout << "Child cannot open the pipe!" 
                         << endl;
                }
            }
            else if (childId > 0) {
                if (int named_pipe_fd = 
                        open(fifo_name.data(), O_RDWR);
                    named_pipe_fd >= 0) { // {10}
                    string message
                        = "Sending some message to the child!";
                    write(named_pipe_fd,
                          message); // {11}

                    sleep(1);
                    message = read(named_pipe_fd); // {12}
                    cout << "Parent: " << message << endl;
                    close(named_pipe_fd);
                }
            }
            else {
                cout << "Fork failed!";
            }
            remove(fifo_name); // {13}
        }
    }
    return 0;
}