#include "ThreadPool.h"
#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#define TASK_NUM 50
int main(void)
{
	auto pool = ThreadPool::getInstance();
	auto start = std::chrono::steady_clock::now();
	std::future<int> result[TASK_NUM];
	std::cout << "CPU Core Size is " << std::thread::hardware_concurrency()<<std::endl;
	for (int i = 0; i < TASK_NUM; i++)
		result[i] = pool->enqueue([i]()->int {
		std::cout << "Task " << i << " has started" << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(5));
		std::cout << "Task " << i << " has finished" << std::endl;
		return i;
			});
	for (int i = 0; i < TASK_NUM; i++)
		std::cout << "Result of task " << i << " is " << result[i].get() << std::endl;
	

	while (!pool->idle());
	std::cout << "The pool is idle" << std::endl;

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "ThreadPool has finished execution in " << elapsed.count() << " seconds\n";
	return 0;
}