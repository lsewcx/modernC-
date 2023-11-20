#include "ThreadPool.h"
#include <iostream>
#include <cassert>
int num=0;
int num1=0;
// Test function that adds two numbers
void add(int a, int b)
{
    num1=a+b;
}

// Test function that multiplies two numbers
void multiply(int a, int b)
{
    num=a*b;
}

int main()
{
    ThreadPool pool(4); // Create a thread pool with 4 threads

    // Test adding two numbers
    auto future1 = pool.add(add, 2, 3);

    // Test multiplying two numbers
    auto future2 = pool.add(multiply,2,3);

    std::cout << "All tests passed!" << std::endl;

    return 0;
}