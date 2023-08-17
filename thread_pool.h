#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <queue>
#include <vector>
#include <functional>

template<class T>
class safe_queue {
public:
	void push(T&& item) {
		std::lock_guard lk(q_mut);
		s_q.push(std::move(item));
		q_cv.notify_all();
	}

	T pop() {
		std::unique_lock lk(q_mut);
		q_cv.wait(lk, [&] { return !s_q.empty(); });

		T item = std::move(s_q.front());
		s_q.pop();

		return std::move(item);
	}

	bool empty() {
		std::lock_guard lk(q_mut);
		return s_q.empty();
	}
private:
	std::queue<T> s_q;
	mutable std::mutex q_mut;
	std::condition_variable q_cv;
};

template<class T>
class thread_pool {
public:
	thread_pool(const thread_pool&) = delete;
	thread_pool(thread_pool&&) = delete;
	thread_pool& operator=(const thread_pool&) = delete;
	thread_pool& operator=(thread_pool&&) = delete;

	thread_pool() {
		for (unsigned int i = 0; i < std::thread::hardware_concurrency(); ++i)
		{
			thrds.push_back(std::thread([this]() { work(); }));
		}
	}
	~thread_pool() {
		for (auto& th : thrds)
			th.join();
	}

	void submit(T&& task) {
		work_q.push(std::move(task));
		cv.notify_one();
	}

	void stop() {
		std::lock_guard guard(mut);
		pursue.store(false);
	}
private:
	void work() {
		while (pursue.load()) {
			std::unique_lock lk(mut);
			cv.wait(lk);
			if (!work_q.empty()) {
				auto foo = work_q.pop();
				foo();
			}
			else {
				std::this_thread::yield();
			}
		}
	}

	std::vector<std::thread> thrds;
	safe_queue<T> work_q;
	std::mutex mut;
	std::condition_variable cv;
	std::atomic<bool> pursue{true};
};