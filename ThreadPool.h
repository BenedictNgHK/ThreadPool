#pragma once
#include <thread>
#include <string>
#include <string_view>
#include <mutex>
#include <future>
#include <memory>
#include <condition_variable>
#include <queue>
#include <vector>
#include <functional>
#include <atomic>
class ThreadPool 
{
public: 
	static std::shared_ptr<ThreadPool> getInstance(unsigned int  threadNo = std::thread::hardware_concurrency() + 1);
	
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool & operator=(const ThreadPool&) = delete;
	~ThreadPool();
	/*template<typename F, class... Args>
	void enqueue(F&& f, Args &&... args)
	{
		
		std::function<void()> task = std::bind(std::forward<F>(f), std::forward<Args>(args)...);

		
		std::unique_lock<std::mutex> lock(mtx);
		tasks.emplace(std::move(task));
		
		lock.unlock();

		cond.notify_one();
		
	}*/
	template<typename F, typename... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
	{
		using return_type = decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);

		std::future<return_type> res = task->get_future();

		{
			std::unique_lock<std::mutex> lock(mtx);
			if (stop)
				throw std::runtime_error("enqueue on stopped ThreadPool");
			tasks.emplace([task]() { (*task)(); });
		}

		cond.notify_one();
		return res;
	}

	bool full();
	bool idle() ;
	
private:
	std::vector<std::thread> threads;
	
	std::queue<std::function<void()>> tasks;
	std::mutex mtx;
	std::condition_variable cond;
	unsigned int thread_no;
	bool stop;
	ThreadPool(unsigned int  threadNo);
	static std::once_flag flag;
	static std::shared_ptr<ThreadPool> instance;
	
	static int active_threads;
};


