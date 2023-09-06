#pragma once

#include <atomic>
#include <vector>
#include <thread>

#include "safe_queue.h"

template<class T>
class thread_pool {
public:
	thread_pool(const thread_pool&) = delete;
	thread_pool(thread_pool&&) = delete;
	thread_pool& operator=(const thread_pool&) = delete;
	thread_pool& operator=(thread_pool&&) = delete;

	thread_pool() {
		const unsigned cores = std::thread::hardware_concurrency();

		try {
			for (unsigned i = 0; i < cores; ++i)
				thrds.push_back(std::thread(&thread_pool::work, this));
		}
		catch (...)
		{
			pursue = false;
			throw;
		}
	}

	~thread_pool() {
		pursue = false;

		for (auto& th : thrds)
			if(th.joinable())
				th.join();
	}

	void submit(T&& task) {
		work_q.push(std::move(task));
	}

	void stop() {
		while (!work_q.empty()) {
			std::this_thread::sleep_for(150ms);
			continue;
		}
		pursue = false;
	}
private:
	using func = std::function<void()>;
	void work() {
		while (pursue) 
		{
			std::shared_ptr<func> foo{ nullptr };
			foo = work_q.try_pop();
			if (foo) 
			{
				(*foo)();
			}
			else 
			{
				std::this_thread::yield();
			}
		}
	}

	std::vector<std::thread> thrds;
	safeQueue<T> work_q;
	std::mutex mut;
	std::atomic<bool> pursue = true;
};