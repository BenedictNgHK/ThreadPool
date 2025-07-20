// ThreadPool.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "ThreadPool.h"
#include <iostream>
std::once_flag ThreadPool::flag;
std::shared_ptr<ThreadPool> ThreadPool::instance = nullptr;
int ThreadPool::active_threads{0};
ThreadPool::ThreadPool(unsigned int  threadNo):thread_no(threadNo), stop(false)
{
	for (unsigned int i = 0; i < thread_no; i++)
		/*threads.emplace_back([this]() {
		while (true)
		{
			std::unique_lock <std::mutex> lock(mtx);
			cond.wait(lock, [this]() {
				return !tasks.empty() || stop;
				});
			if (stop && tasks.empty()) return;
			auto task = std::move(tasks.front());
			tasks.pop();
			lock.unlock();
			task();
		}
		});*/
		threads.emplace_back([this]() {
		while (true)
		{
			std::function<void()> task;

			{
				std::unique_lock<std::mutex> lock(mtx);
				cond.wait(lock, [this]() {
					return !tasks.empty() || stop;
					});
				if (stop && tasks.empty())
					return;

				task = std::move(tasks.front());
				tasks.pop();
				active_threads++;  // Increment active count before running the task
			}

			task(); // Execute task

			active_threads--; // Decrement active count after task completes
		}
			});

}

ThreadPool::~ThreadPool()
{
	
	std::unique_lock<std::mutex> lock(mtx);
	stop = true;
	lock.unlock();
	cond.notify_all();
	for (auto & t : threads)
	{
		t.join();
	}
	

}
std::shared_ptr<ThreadPool> ThreadPool::getInstance(unsigned int  threadNo)
{
	std::call_once(flag, [threadNo]() {
		instance = std::shared_ptr<ThreadPool>(new ThreadPool(threadNo));
		});

	return instance;
}
bool ThreadPool::full()
{
	std::unique_lock<std::mutex> lock(mtx);
	return tasks.size() >= thread_no;
}
bool ThreadPool::idle() 
{
	std::lock_guard<std::mutex> lock(mtx);
	return active_threads == 0 && tasks.empty();
}