#pragma once
#include <thread>
#include <string>
#include <string_view>
#include <mutex>
#include <future>
#include <memory>
#include <condition_variable>
#include <deque>
#include <vector>
#include <functional>
#include <atomic>
#include <random>

class ThreadPool 
{
public: 
	static std::shared_ptr<ThreadPool> getInstance(unsigned int threadNo = std::thread::hardware_concurrency());

	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;
	~ThreadPool();

	template<typename F, typename... Args>
	auto enqueue(F&& f, Args&&... args) -> std::future<decltype(f(args...))>
	{
		using return_type = decltype(f(args...));
		auto task = std::make_shared<std::packaged_task<return_type()>>(
			std::bind(std::forward<F>(f), std::forward<Args>(args)...)
		);
		std::future<return_type> res = task->get_future();

		size_t idx = next_worker++ % thread_no;

		{
			std::lock_guard<std::mutex> lock(queue_mutexes[idx]);
			queues[idx].emplace_front([task]() { (*task)(); });
		}

		conds[idx].notify_one();
		return res;
	}

	bool idle() const;
	int working_threads() const;

private:
	ThreadPool(unsigned int threadNo);
	void worker_loop(size_t index);

	static std::once_flag flag;
	static std::shared_ptr<ThreadPool> instance;

	unsigned int thread_no;
	bool stop = false;

	std::vector<std::thread> threads;
	std::vector<std::deque<std::function<void()>>> queues;
	std::vector<std::mutex> queue_mutexes;
	std::vector<std::condition_variable> conds;

	std::atomic<size_t> next_worker{0};
	static std::atomic<int> active_threads;
};
