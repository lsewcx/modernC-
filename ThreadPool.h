#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>

class ThreadPool
{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    std::condition_variable cv;
    bool stop;

public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    // void add(std::function<void()>);
    template <class F, class... Args>
    auto add(F &&f, Args &&...args)
        -> std::future<typename std::result_of<F(Args...)>::type>;
};
ThreadPool::ThreadPool(int size) : stop(false)
{
    for (int i = 0; i < size; ++i)
    {
        threads.emplace_back(std::thread([this]()
                                         {
            while(true){
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mtx);
                    cv.wait(lock, [this](){
                        return stop || !tasks.empty();
                    });
                    if(stop && tasks.empty()) return;
                    task = tasks.front();
                    tasks.pop();
                }
                task();
            } }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }
    cv.notify_all();
    for (std::thread &th : threads)
    {
        if (th.joinable())
            th.join();
    }
}

// 不能放在cpp文件，原因是C++编译器不支持模版的分离编译
/**
 * Adds a task to the thread pool.
 *
 * @tparam F The type of the task function.
 * @tparam Args The types of the arguments to the task function.
 * @param f The task function to be executed.
 * @param args The arguments to be passed to the task function.
 * @return A std::future object representing the result of the task.
 * @throws std::runtime_error if the thread pool has been stopped.
 */
template <class F, class... Args>
auto ThreadPool::add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);

        // don't allow enqueueing after stopping the pool
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task]()
                      { (*task)(); });
    }
    cv.notify_one();
    return res;
}