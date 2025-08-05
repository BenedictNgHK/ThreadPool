#include "ThreadPool.h"
#include <iostream>
#include <thread>
#include <future>
#include <chrono>


int fibo(int n) 
{
	if (n <= 2) return  1;
	return fibo(n - 1) + fibo(n - 2);
}
int main(int argc, char* argv[])
{
	int TASK_NUM = argv[1] ? std::atoi(argv[1]) : 10; // Default to 10 tasks if no argument is provided
	auto pool = ThreadPool::getInstance();
	auto start = std::chrono::steady_clock::now();
	std::vector<std::future<int>> result(TASK_NUM);
	std::cout << "CPU Core Size is " << std::thread::hardware_concurrency()<<std::endl;
	for (int i = 0; i < TASK_NUM; i++)
	{
		std::cout << "Enqueuing task " << i << std::endl;
		result[i] = pool->enqueue([i,TASK_NUM](int){
			std::cout << "Task " << i << " is running" << std::endl;
			int result = fibo(TASK_NUM- i); // Simulate a CPU-intensive task
			std::cout << "Task " << i << " is finished" << std::endl;
			return result;

		}, i); // Enqueue tasks with varying input
	}
		for (int i = 0; i < TASK_NUM; i++)
		std::cout << "Result of task " << i << " is " << result[i].get() << std::endl;
	

	while (!pool->idle());
	std::cout << "The pool is idle" << std::endl;

	auto end = std::chrono::steady_clock::now();
	std::chrono::duration<double> elapsed = end - start;
	std::cout << "ThreadPool has finished execution in " << elapsed.count() << " seconds\n";
	return 0;
}