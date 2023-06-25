#include <iostream>
#include <semaphore>
#include <thread>

using namespace std;

atomic<uint16_t> shared_resource = 0;

binary_semaphore sem_to_produce(0);
binary_semaphore sem_to_consume(0);

constexpr uint16_t limit = 65536;

void producer() {
    for (auto i = 0; i < limit; i++) {
        sem_to_produce.acquire();
        ++shared_resource;
        cout << "Producing value: " << shared_resource << endl;
        sem_to_consume.release();
        cout << "Producer finished!" << endl;
    }
}

void consumer() {
    for (auto i = 0; i <= limit; i++) {
        cout << "Waiting for data..." << endl;
        sem_to_consume.acquire();
        --shared_resource;
        cout << "Value after consumer: " << shared_resource << endl;
        sem_to_produce.release();
        cout << "Consumer finished!" << endl;
    }
}

int main() {
    sem_to_produce.release();
    std::thread t1(producer);
    std::thread t2(consumer);

    t1.join();
    t2.join();
}

