#include <iostream>
#include <syncstream>
#include <semaphore>
#include <thread>

using namespace std;

uint32_t shared_resource = 0;

binary_semaphore sem_to_produce(0);
binary_semaphore sem_to_consume(0);

constexpr uint32_t limit = 65536;

void producer() {
    for (auto i = 0; i < limit; i++) {
        sem_to_produce.acquire();
        ++shared_resource;
        osyncstream(cout) << "Producing value: " 
                          << shared_resource << endl;
        sem_to_consume.release();
        osyncstream(cout) << "Producer finished!" << endl;
    }
}

void consumer() {
    for (auto i = 0; i <= limit; i++) {
        osyncstream(cout) << "Waiting for data..." << endl;
        sem_to_consume.acquire();
        --shared_resource;
        osyncstream(cout) << "Value after consumer: " << shared_resource << endl;
        sem_to_produce.release();
        osyncstream(cout) << "Consumer finished!" << endl;
    }
}

int main() {
    sem_to_produce.release();
    jthread t1(producer);
    jthread t2(consumer);

    t1.join();
    t2.join();
}
