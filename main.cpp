#include <iostream>
#include <chrono>

#include "thread_pool.h"

using namespace std::chrono_literals;

static int count = 1;

std::mutex m;

void black() {
	std::this_thread::sleep_for(300ms);
	std::unique_lock guard (m);
	std::cout << count << ") Thread " << std::this_thread::get_id() << " started..." << std::endl;
	std::cout << __FUNCTION__ << std::endl;
	count++;
}

void white() {
	std::this_thread::sleep_for(300ms);
	std::unique_lock guard (m);
	std::cout << count << ") Thread " << std::this_thread::get_id() << " started..." << std::endl;
	std::cout << __FUNCTION__ << std::endl;
	count++;
}

int main([[maybe_unused]] int argc, [[maybe_unused]] char** argv) {
	safe_queue<std::function<void(void)>> work_q;
	for (int i = 6; i; --i) {
		std::function<void(void)> task1(black);
		work_q.push(std::move(task1));
		std::function<void(void)> task2(white);
		work_q.push(std::move(task2));
	}

	thread_pool<std::function<void(void)>> pool(work_q);

	for(int i = 4; i; --i)
	{
		//std::this_thread::sleep_for(0.5s);
		std::function<void(void)> task1(black);
		pool.submit(std::move(task1));
		std::function<void(void)> task2(white);
		pool.submit(std::move(task2));
	}

	pool.stop();

	return 0;
}