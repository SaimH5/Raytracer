#ifndef _THREAD_POOL_h
#define _THREAD_POOL_h

#include <iostream>     
#include <atomic>
#include <future>
#include <mutex>
#include <queue>
#include <thread>
#include <tuple>

// Wrapper class for callable objects
class func_wrapper
{
    struct base
    {
        virtual void call()=0;
    };

    std::unique_ptr<base> heldFunc;

    template<typename F, typename... Args>
    struct impl : base
    {
        F m_f;
        std::tuple<Args...> m_args;
        impl(F&& f, Args... args) : m_f(std::move(f)), m_args(std::move(args)...) {}

        template<size_t... Idxs>
        void call_helper(std::index_sequence<Idxs...>)
        {
            m_f(std::get<Idxs>(m_args)...);
        }

        virtual void call() override { call_helper(std::index_sequence_for<Args...>{}); }
    };

public:
    template<typename F, typename... Args>
    func_wrapper(F&& f_, Args... arg) : heldFunc(std::make_unique<impl<F, Args...>>(std::move(f_), std::move(arg)...)) {}

    func_wrapper()=default;

    func_wrapper(func_wrapper&& other) : heldFunc(std::move(other.heldFunc)) {}

    func_wrapper& operator=(func_wrapper&& other)
    {
        heldFunc = std::move(other.heldFunc);
        return *this;
    }

    void operator()() { heldFunc->call(); }
};


// Thread pool class that adds incoming tasks to a queue and assigns them
// to available worker threads based on hardware support
class thread_pool
{
    std::atomic_bool done;
    std::queue<func_wrapper> m_tasks_queue;
    std::vector<std::thread> m_threads;
    std::mutex m_mtx;
    std::condition_variable cv_wait_for_tasks;

    // Function run by all worker threads
    // Waits for tasks to be available using the condition variable
    // Takes a task from the queue and calls it when notified
    // Terminates when the destructor is called
    void worker()
    {
        while(!done)
        {
            std::unique_lock lck(m_mtx);
            cv_wait_for_tasks.wait(lck, [this]() { return !m_tasks_queue.empty() || done; });
            if(!m_tasks_queue.empty())
            {
                func_wrapper task(std::move(m_tasks_queue.front()));
                m_tasks_queue.pop();
                lck.unlock();
                task();
            }
        }
    }

public:
    // Constructor; Creates threads according to available cores/threads on the hardware
    // Creates one less thread than the total available to allow main thread of the program to also do work
    thread_pool() : done(false) 
    {
        size_t thread_count = std::thread::hardware_concurrency();
        thread_count = thread_count > 0 ? thread_count : 1;
        try
        {
            for(unsigned int i = 0; i < thread_count - 1; i++)
            {
                m_threads.push_back(std::thread(&worker, this));
            }
        }
        catch(...)
        {
            done = true;
            throw;
        }
    }

    ~thread_pool()
    {
        done = true;
        cv_wait_for_tasks.notify_all();
        for(auto& thread : m_threads)
        {
            if(thread.joinable())
            {
                thread.join();
            }
        }
    }

    unsigned int thread_count() const { return m_threads.size(); }

    // Templated submit function that takes in arbitrary callable objects
    // creates a packaged_task out of them and adds them to the main queue,
    // then returns a future of the appropriate type
    template<typename F, typename... Args>
    std::future<std::result_of_t<F(Args...)>> submit(F f, Args... arg)
    {
        typedef typename std::result_of_t<F(Args...)> res_type;
        std::packaged_task<res_type(Args...)> p_task(f);
        std::future<res_type> ft = p_task.get_future(); 

        std::lock_guard lck(m_mtx);
        m_tasks_queue.push(func_wrapper(std::move(p_task), std::move(arg)...));

        cv_wait_for_tasks.notify_one();
        return ft;
    }
};

#endif
