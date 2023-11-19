#include "ThreadPool.h"
#include <iostream>
#include <cassert>

// Test function that adds two numbers
int add(int a, int b)
{
    return a + b;
}

// Test function that multiplies two numbers
int multiply(int a, int b)
{
    return a * b;
}

int main()
{
    ThreadPool pool(4); // Create a thread pool with 4 threads

    // Test adding two numbers
    auto future1 = pool.add(add, 2, 3);
    assert(future1.get() == 5);

    // Test multiplying two numbers
    auto future2 = pool.add(multiply, 4, 5);
    assert(future2.get() == 20);

    std::cout << "All tests passed!" << std::endl;

    return 0;
}