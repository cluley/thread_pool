#include <iostream>
#include <chrono>

#include "thread_pool.h"

using namespace std::chrono_literals;

void black() {
	std::cout << "Thread " << std::this_thread::get_id() << " started..." << std::endl;
	std::cout << __FUNCTION__ << std::endl;
}

void white() {
	std::cout << "Thread " << std::this_thread::get_id() << " started..." << std::endl;
	std::cout << __FUNCTION__ << std::endl;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	thread_pool<std::function<void(void)>> pool;

	for(int i = 0; i < 20; ++i)
	{
		std::this_thread::sleep_for(1s);
		std::function<void(void)> task1(black);
		pool.submit(std::move(task1));
		std::function<void(void)> task2(white);
		pool.submit(std::move(task2));
	}

	return 0;
}