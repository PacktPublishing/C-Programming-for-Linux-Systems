#include <array>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <iostream>
#include <sys/mman.h>
#include <sys/wait.h>
#include <semaphore>
#include <string_view>

using namespace std;

string_view SHM_ID      = "/test_shm";
string_view SEM_PROD_ID = "/test_sem_prod";
string_view SEM_CONS_ID = "/test_sem_cons";

constexpr auto SHM_SIZE = 1024;

sem_t *sem_prod;
sem_t *sem_cons;

void process_creator() {
    int res        = 0;
    int fd         = 0;
    void *shm_addr = NULL;

    if (int pid = fork(); pid == 0) {
        // Child - used for consuming data.
        if (fd = shm_open(SHM_ID.data(), 
                          O_RDONLY, 
                          0700); // {1}
            fd == -1) {
            const auto ecode{make_error_code(errc{errno})};
            cerr << "Error opening shm region";
            system_error exception{ecode}; 
            throw exception;
        }

        shm_addr = mmap(NULL, SHM_SIZE,
                        PROT_READ, MAP_SHARED,
                        fd, 0); // {2}
        if (shm_addr == MAP_FAILED) {
            const auto ecode{make_error_code(errc{errno})};
            cerr << "Error mapping shm region";
            system_error exception{ecode};
            throw exception;
        }

        array<char, SHM_SIZE> buffer{};
        sem_wait(sem_cons);
        memcpy(buffer.data(),
               shm_addr, 
               buffer.size()); // {3}
        if(strlen(buffer.data()) != 0) {
            cout << "PID : " << getpid()
                 << "consumed: " << buffer.data();
        }
        sem_post(sem_prod);

        exit(EXIT_SUCCESS);
    }
    else if (pid > 0) {
        // Parent - used for producing data.
        fd = shm_open(SHM_ID.data(), 
                      O_CREAT | O_RDWR, 
                      0700); // {4}
        if (fd == -1) {
            const auto ecode{make_error_code(errc{errno})};
            cerr << "Error opening shm region";
            system_error exception{ecode};
            throw exception;
        }

        res = ftruncate(fd, SHM_SIZE); // {5}
        if (res == -1) {
            const auto ecode{make_error_code(errc{errno})};
            cerr << "Error resizing shm region";
            system_error exception{ecode};
            throw exception;
        }

        shm_addr = mmap(NULL, SHM_SIZE,
                        PROT_WRITE, MAP_SHARED,
                        fd, 0); // {6}
        if (shm_addr == MAP_FAILED) {
            const auto ecode{make_error_code(errc{errno})};
            cerr << "Error mapping shm region";
            system_error exception{ecode};
            throw exception;
        }
        sem_wait(sem_prod);
        string_view produced_data {"Some test data, coming!"};
        memcpy(shm_addr, 
               produced_data.data(), 
               produced_data.size()); // {7}
        sem_post(sem_cons);

        waitpid(pid, NULL, 0); // {8}
        res = munmap(shm_addr, SHM_SIZE); // {9}
        if (res == -1) {
            const auto ecode{make_error_code(errc{errno})};
            cerr << "Error cleaning-up shm region";
            system_error exception{ecode};
            throw exception;
        }

        fd = shm_unlink(SHM_ID.data()); //{10}
        if (fd == -1) {
            const auto ecode{make_error_code(errc{errno})};
            cerr << "Error destructing shm region";
            system_error exception{ecode};
            throw exception;
        }
    }
    else {
        const auto ecode{make_error_code(errc{errno})};
        cerr << "Process creation failed!";
        system_error exception{ecode};
        throw exception;
    }
    exit(EXIT_SUCCESS);
}

int main() {
    sem_prod = sem_open(SEM_PROD_ID.data(), 
                        O_CREAT, 0644, 0); // {11}
    sem_cons = sem_open(SEM_CONS_ID.data(), 
                        O_CREAT, 0644, 0);
    sem_post(sem_prod);
    process_creator();
    sem_close(sem_prod); // {12}
    sem_close(sem_cons);
    sem_unlink(SEM_PROD_ID.data());
    sem_unlink(SEM_CONS_ID.data());
    return 0;
}