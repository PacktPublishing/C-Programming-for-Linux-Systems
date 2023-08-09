#include <array>
#include <iostream>
#include <mqueue.h>
#include <cstring>
#include <string_view>
#include <thread>
#include <unistd.h>
#include <cerrno>

using namespace std;

constexpr auto MAX_SIZE = 1024;
string_view QUEUE_NAME  = "/test_queue";

void readFromQueue() {
    cout << "Thread READER starting..." << endl;

    mqd_t          mq   = { 0 };
    struct mq_attr attr = { 0 };

    array<char, MAX_SIZE> buffer{};

    // Initialize the queue attributes.
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    // Check if the queue open is successful.
    if (mq = mq_open(QUEUE_NAME.data(), O_CREAT | O_RDONLY, 
                     0700, &attr); mq > -1) { // {1}
        for (;;) {
            if (auto bytes_read = mq_receive(mq, 
                                             buffer.data(),
                                             buffer.size(),
                                             NULL);
                                  bytes_read > 0) { // {2}
                buffer[bytes_read] = '\0';
                cout << "Received: " 
                     << buffer.data() 
                     << endl; // {3}
            }
            else if (bytes_read == -1) {
                perror("Receive message failed!");
            }
            else {
                cout << "\n\n\n***Receiving ends***" 
                     << endl;
                mq_close(mq); // {4}
                break;
            }
        }
    }
    else {
        cout << "Receiver: Failed to load queue: "
             << strerror(errno) << endl;
    }

    mq_unlink(QUEUE_NAME.data());
}

void writeToQueue() {
    cout << "Thread WRITER starting..." << endl;

    mqd_t mq = { 0 };

    if (mq = mq_open(QUEUE_NAME.data(), O_WRONLY, 
                     0700, NULL); mq > -1) { // {5}
        int fd = open("test.dat", O_RDONLY); // {6}
        if (fd > 0) {
            for (;;) {
                // This could be taken from cin.
                array<char, MAX_SIZE> buffer{};
                if (auto bytes_to_send =
                        read(fd, 
                             buffer.data(), 
                             buffer.size());
                             bytes_to_send > 0) { // {7}
                    if (auto b_sent = 
                            mq_send(mq,
                                    buffer.data(),
                                    buffer.size(),
                                    0); 
                            b_sent == -1) {// {8}
                        cout << "Sent failed!"
                            << strerror(errno)
                            << endl;
                    }
                }
                else if (bytes_to_send == 0) {
                    cout << "Sending ends...." << endl;
                    if (auto b_sent = 
                            mq_send(mq,
                                    buffer.data(),
                                    0,
                                    0); b_sent == -1) { // {9}
                        cout << "Sent failed!"
                            << strerror(errno)
                            << endl;
                    }
                    break;
                }
                else {
                    perror("Reading from file failed!");
                }
            }
        }
        else {
            cout << "Error opening file" << endl;
        }
        close(fd); // {10}
        mq_close(mq);
    }
    else {
        cout << "Sender: Failed to load queue: "
            << strerror(errno) << endl;
    }
}

int main() {
    thread reader{ readFromQueue };
    thread writer{ writeToQueue };

    reader.join();
    writer.join();

    cout << "Main: program completed. Exiting." << endl;
    return 0;
}
