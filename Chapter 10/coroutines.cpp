#include <coroutine>

using namespace std;

struct Task {
    struct promise_type {
        using Handle = coroutine_handle<promise_type>;
        Task get_return_object() {
            return Task { Handle::from_promise(*this) };
        }
        suspend_always 
            initial_suspend() { return {}; }
        suspend_never 
            final_suspend() noexcept { return {}; }
        void return_void() { }
        void unhandled_exception() { }
    };

    explicit Task (promise_type::Handle crtHdnl) : 
                       crtHandle(crtHdnl) {}
    void destroy() { crtHandle.destroy(); }
    void resume() { crtHandle.resume(); }

    private:
        promise_type::Handle crtHandle;
};

Task exCoroutine() {
    co_return;
}

int main() {
    auto async_task = exCoroutine();
    async_task.resume();
    //async_task.destroy();
}