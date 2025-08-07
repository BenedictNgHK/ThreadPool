#include "ThreadPool.h"
#include <iostream>
#include <random>

std::once_flag ThreadPool::flag;
std::shared_ptr<ThreadPool> ThreadPool::instance = nullptr;
std::atomic<int> ThreadPool::active_threads{0};

ThreadPool::ThreadPool(unsigned int threadNo)
    : thread_no(threadNo), stop(false),
      queues(threadNo), queue_mutexes(threadNo), conds(threadNo)
{
	for (size_t i = 0; i < thread_no; ++i) {
		threads.emplace_back([this, i]() {
			worker_loop(i);
		});
	}
}

void ThreadPool::worker_loop(size_t index)
{
	std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<size_t> dist(0, thread_no - 1);

	while (true)
	{
		std::function<void()> task;

		{
			std::unique_lock<std::mutex> lock(queue_mutexes[index]);

			conds[index].wait(lock, [this, index] {
				return stop || !queues[index].empty();
			});

			if (stop && queues[index].empty())
				break;

			if (!queues[index].empty()) {
				task = std::move(queues[index].front());
				queues[index].pop_front();
			}
		}

		// Try to steal if no task
		if (!task) {
			for (int attempt = 0; attempt < thread_no; ++attempt) {
				size_t victim = dist(rng);
				if (victim == index) continue;

				std::lock_guard<std::mutex> lock(queue_mutexes[victim]);
				if (!queues[victim].empty()) {
					task = std::move(queues[victim].back());
					queues[victim].pop_back();
					break;
				}
			}
		}

		if (task) {
			active_threads++;
			task();
			active_threads--;
		}
	}
}

ThreadPool::~ThreadPool()
{
	stop = true;
	for (auto& cond : conds)
		cond.notify_all();

	for (auto& t : threads)
		if (t.joinable())
			t.join();
}

std::shared_ptr<ThreadPool> ThreadPool::getInstance(unsigned int threadNo)
{
	std::call_once(flag, [threadNo]() {
		instance = std::shared_ptr<ThreadPool>(new ThreadPool(threadNo));
	});
	return instance;
}

bool ThreadPool::idle() const
{
	return active_threads == 0;
}

int ThreadPool::working_threads() const
{
	return active_threads.load();
}
