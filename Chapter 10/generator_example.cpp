#include <coroutine>
#include <iostream>

using namespace std;

struct Generator {
    struct promise_type {
        using Handle = coroutine_handle<promise_type>;
        Generator get_return_object() {
            return Generator
                { Handle::from_promise(*this) };
        }
        suspend_always initial_suspend()
            { return {}; }
        suspend_always final_suspend() noexcept 
            { return {}; }
        suspend_always yield_value(auto value) {
            currValue = value;
            return {};
        }
        void unhandled_exception() { 
            exit(EXIT_FAILURE);
        }
        uint32_t currValue;
    };

    explicit Generator(promise_type::Handle p_crtHdnl) : 
        crtHndl(p_crtHdnl) {}

    ~Generator() {
        if (crtHndl)
            crtHndl.destroy();
    }

    int next() {
        crtHndl.resume();
        return crtHndl.promise().currValue;
    }

private:
    promise_type::Handle crtHndl;
};

Generator exCoroutine() {
    auto idx = 0;
    for (;;) {
        co_yield idx++;
    }
}

int main() {
    auto crt = exCoroutine();

    for (auto idx = 0; (idx = crt.next()) < 100000; ) 
        cout << idx << " ";

    cout << endl;
    return 0;
}
