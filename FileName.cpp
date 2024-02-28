#include <iostream>
#include <omp.h>
#include <vector>
#include <algorithm>
#include <mutex>

int main() {
    int k, N;
    std::cout << "Enter the number of threads k: ";
    std::cin >> k;
    std::cout << "Enter the value of N: ";
    std::cin >> N;

    std::vector<int> numbers(N);
    for (int i = 0; i < N; ++i) {
        numbers[i] = i + 1;
    }

    int actualThreads = 0;
    if (k < N) {
        actualThreads = std::min(k, N);
    }
    else {
        actualThreads = std::max(k, N);
    }

    std::vector<int> results(actualThreads, 0);

    int totalSum = 0;

    std::mutex outputMutex;

#pragma omp parallel num_threads(actualThreads) reduction(+:totalSum)
    {
        int rank = omp_get_thread_num();
        int sum = 0;

#pragma omp for schedule(static) 
        for (int i = 0; i < N; ++i) {
            sum += numbers[i];
        }

#pragma omp critical
        {
            results[rank] = sum;
        }

#pragma omp barrier 

#pragma omp master
        {
            std::sort(results.begin(), results.end(), [&](int a, int b) {
                return omp_get_thread_num() < omp_get_thread_num();
                });
   
            for (int i = 0; i < actualThreads; ++i) {
                std::lock_guard<std::mutex> lock(outputMutex);
                std::cout << "[" << i << "]: Sum = " << results[i] << std::endl;
                totalSum += results[i];
            }

            std::cout << "Sum = " << totalSum << std::endl;
        }
    }

    return 0;
}
