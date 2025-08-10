#include <ThreadPool.h>
#include <iostream>
#include <cmath>
void complex_task(int n, int complexity) {
    volatile double result = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < complexity; ++j) {
            result += std::sqrt(i * j + 1.0); // Some floating-point computation
        }
    }
}
void test_imbalanced_workload() {
    const int TOTAL_TASKS = 32;
    const int THREAD_COUNT = 4;
    auto pool = ThreadPool::getInstance(THREAD_COUNT);
    std::vector<std::future<void>> results;

    auto start = std::chrono::high_resolution_clock::now();

    // First few tasks are very heavy, rest are light
    for (int i = 0; i < TOTAL_TASKS; ++i) {
        if (i < THREAD_COUNT) {
            results.push_back(pool->enqueue([i] {
                std::cout << "Heavy task " << i << " started on thread " << std::this_thread::get_id() << std::endl;
                complex_task(100000, 1000); // Very heavy
                std::cout << "Heavy task " << i << " completed\n";
            }));
        } else {
            results.push_back(pool->enqueue([i] {
                std::cout << "Light task " << i << " started on thread " << std::this_thread::get_id() << std::endl;
                complex_task(1000, 100); // Light
                std::cout << "Light task " << i << " completed\n";
            }));
        }
    }

    for (auto& result : results) result.get();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[Imbalanced] Total execution time: " << duration.count() << "ms\n";
}

void test_balanced_workload() {
    const int TOTAL_TASKS = 32;
    const int THREAD_COUNT = 4;
    auto pool = ThreadPool::getInstance(THREAD_COUNT);
    std::vector<std::future<void>> results;

    auto start = std::chrono::high_resolution_clock::now();

    // All tasks are equally heavy
    for (int i = 0; i < TOTAL_TASKS; ++i) {
        results.push_back(pool->enqueue([i] {
            std::cout << "Balanced task " << i << " started on thread " << std::this_thread::get_id() << std::endl;
            complex_task(5000, 1000); // All tasks same
            std::cout << "Balanced task " << i << " completed\n";
        }));
    }

    for (auto& result : results) result.get();

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
    std::cout << "[Balanced] Total execution time: " << duration.count() << "ms\n";
}

int main() {
    std::cout << "Testing workload balance with " 
              << std::thread::hardware_concurrency() 
              << " hardware threads available\n\n";

    std::cout << "=== Imbalanced Workload Test ===\n";
    test_imbalanced_workload();

    // std::cout << "\n=== Balanced Workload Test ===\n";
    // test_balanced_workload();

    return 0;
}