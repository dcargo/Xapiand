/*
 * Copyright (c) 2018,2019 Dubalu LLC
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <chrono>
#include <condition_variable>

#include "concurrent_queue.h"
#include "likely.h"

using namespace std::chrono_literals;

template <typename T>
class BlockingConcurrentQueue : public ConcurrentQueue<T> {
	std::condition_variable cond;

public:
	BlockingConcurrentQueue() :
		ConcurrentQueue<T>() {}

	BlockingConcurrentQueue(size_t) :
		ConcurrentQueue<T>() {}

	bool enqueue(const T& item) {
		if likely(ConcurrentQueue<T>::enqueue(item)) {
			cond.notify_one();
			return true;
		}
		return false;
	}

	bool enqueue(T&& item) {
		if likely(ConcurrentQueue<T>::enqueue(std::forward<T>(item))) {
			cond.notify_one();
			return true;
		}
		return false;
	}

	template<typename It>
	bool enqueue_bulk(It itemFirst, size_t count) {
		if likely(ConcurrentQueue<T>::enqueue_bulk(itemFirst, count)) {
			cond.notify_all();
			return true;
		}
		return false;
	}

	template<typename U>
	void wait_dequeue(U& item) {
		std::unique_lock<std::mutex> lk(*ConcurrentQueue<T>::mtx);
		cond.wait(lk, [&]{
			return !ConcurrentQueue<T>::queue.empty();
		});
		item = std::move(ConcurrentQueue<T>::queue.front());
		ConcurrentQueue<T>::queue.pop_front();
	}

	template<typename U>
	bool wait_dequeue_timed(U& item, int64_t timeout_usecs = -1) {
		std::unique_lock<std::mutex> lk(*ConcurrentQueue<T>::mtx);
		auto wait_pred = [&]{
			return !ConcurrentQueue<T>::queue.empty();
		};
		if (timeout_usecs) {
			if (timeout_usecs > 0) {
				auto timeout_tp = std::chrono::steady_clock::now() + std::chrono::duration<double>(timeout_usecs / 1e6);
				if (!cond.wait_until(lk, timeout_tp, wait_pred)) {
					return false;
				}
			} else {
				while (!cond.wait_for(lk, 1s, wait_pred)) {}
			}
		} else {
			if (!wait_pred()) {
				return false;
			}
		}
		item = std::move(ConcurrentQueue<T>::queue.front());
		ConcurrentQueue<T>::queue.pop_front();
		return true;
	}
};
