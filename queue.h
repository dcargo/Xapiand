/*
 * Copyright (C) 2015 deipi.com LLC and contributors. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#ifndef XAPIAND_INCLUDED_QUEUE_H
#define XAPIAND_INCLUDED_QUEUE_H

#include <cerrno>
#include <deque>
#include <list>

#ifdef HAVE_CXX11
#  include <unordered_map>
#else
#  include <map>
#endif

#include <sys/time.h>
#include <pthread.h>


template<class T, class Container = std::deque<T> >
class Queue {
private:
	struct timespec _ts;

protected:
	Container _items_queue;

	// A mutex object to control access to the underlying queue object
	pthread_mutex_t _qmtx;
	pthread_mutexattr_t _qmtx_attr;

	// A variable condition to make threads wait on specified condition values
	pthread_cond_t _push_cond;
	pthread_cond_t _pop_cond;

	bool _finished;
	size_t _limit;

	struct timespec & _timespec(double timeout) {
		struct timeval tv;
		gettimeofday(&tv, NULL);
		_ts.tv_sec = tv.tv_sec + int(timeout);
		_ts.tv_nsec = int((timeout - int(timeout)) * 1e9);
		return _ts;
	}

	virtual bool _push(const T & element, double timeout) {
		struct timespec *timeout_ts = (timeout > 0.0) ? &_timespec(timeout) : NULL;

		if (!_finished) {
			size_t size = _items_queue.size();
			while (_limit >= 0 && size >= _limit) {
				if (!_finished && timeout) {
					if (timeout_ts) {
						if (pthread_cond_timedwait(&_pop_cond, &_qmtx, timeout_ts) == ETIMEDOUT) {
							return false;
						}
					} else {
						pthread_cond_wait(&_pop_cond, &_qmtx);
					}
				} else {
					return false;
				}
				size = _items_queue.size();
			}
			// Insert the element in the FIFO queue
			_items_queue.push_front(element);
		}

		return true;
	}

	virtual bool _pop(T & element, double timeout) {
		struct timespec *timeout_ts = (timeout > 0.0) ? &_timespec(timeout) : NULL;

		// While the queue is empty, make the thread that runs this wait
		while(_items_queue.empty()) {
			if (!_finished && timeout) {
				if (timeout_ts) {
					if (pthread_cond_timedwait(&_push_cond, &_qmtx, timeout_ts) == ETIMEDOUT) {
						return false;
					}
				} else {
					pthread_cond_wait(&_push_cond, &_qmtx);
				}
			} else {
				return false;
			}
		}

		//when the condition variable is unlocked, popped the element
		element = _items_queue.front();

		//pop the element
		_items_queue.pop_front();

		return true;
	}
	virtual void _clear() {
		_items_queue.clear();
	}

public:
	Queue(size_t limit=-1)
		: _finished(false),
		  _limit(limit) {
		pthread_cond_init(&_push_cond, 0);
		pthread_cond_init(&_pop_cond, 0);

		pthread_mutexattr_init(&_qmtx_attr);
		pthread_mutexattr_settype(&_qmtx_attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&_qmtx, &_qmtx_attr);
	}

	~Queue() {
		finish();

		pthread_mutex_destroy(&_qmtx);
		pthread_mutexattr_destroy(&_qmtx_attr);

		pthread_cond_destroy(&_push_cond);
		pthread_cond_destroy(&_pop_cond);
	}

	void finish() {
		pthread_mutex_lock(&_qmtx);
		if (_finished) {
			pthread_mutex_unlock(&_qmtx);
			return;
		}
		_finished = true;
		_clear();
		pthread_mutex_unlock(&_qmtx);

		// Signal the condition variable in case any threads are waiting
		pthread_cond_broadcast(&_push_cond);
		pthread_cond_broadcast(&_pop_cond);
	}

	bool push(const T & element, double timeout=-1.0) {
		pthread_mutex_lock(&_qmtx);
		bool pushed = _push(element, timeout);
		pthread_mutex_unlock(&_qmtx);

		if (pushed) {
			// Notifiy waiting thread it can pop now
			pthread_cond_signal(&_push_cond);
		}

		return pushed;
	}

	bool pop(T & element, double timeout=-1.0) {
		pthread_mutex_lock(&_qmtx);
		bool popped = _pop(element, timeout);
		pthread_mutex_unlock(&_qmtx);

		if (popped) {
			// Notifiy waiting thread it can push/push now
			pthread_cond_signal(&_push_cond);
			pthread_cond_signal(&_pop_cond);
		}

		return popped;
	}

	void clear() {
		pthread_mutex_lock(&_qmtx);
		_clear();
		pthread_mutex_unlock(&_qmtx);

		// Notifiy waiting thread it can push/push now
		pthread_cond_signal(&_push_cond);
		pthread_cond_signal(&_pop_cond);
	}

	bool empty() {
		pthread_mutex_lock(&_qmtx);
		bool empty = _items_queue.empty();
		pthread_mutex_unlock(&_qmtx);
		return empty;
	}

	size_t size() {
		pthread_mutex_lock(&_qmtx);
		size_t size = _items_queue.size();
		pthread_mutex_unlock(&_qmtx);
		return size;
	}

	T & front() {
		pthread_mutex_lock(&_qmtx);
		T & front = _items_queue.front();
		pthread_mutex_unlock(&_qmtx);
		return front;
	}
};



// A Queue with unique values
template<class Key, class T = Key>
class QueueSet : public Queue<std::pair<Key, T>, std::list<std::pair<Key, T> > > {
	typedef Queue<std::pair<Key, T>, std::list<std::pair<Key, T> > > Queue_t;
	typedef std::pair<Key, T> key_value_pair_t;
	typedef typename std::list<key_value_pair_t>::iterator list_iterator_t;
#ifdef HAVE_CXX11
	typedef std::unordered_map<Key, list_iterator_t> queue_map_t;
#else
	typedef std::map<Key, list_iterator_t> queue_map_t;
#endif
	typedef typename queue_map_t::iterator map_iterator_t;

protected:
	enum duplicate_action {
		update,  // Updates the content of the item in the queue
		leave,   // Leaves the old item in the queue
		renew    // Renews the item in the queue (also moving it to front)
	};

	queue_map_t _items_map;

	duplicate_action on_dup(T & val) {
		return renew;
	}

	bool _push(const key_value_pair_t & p, double timeout) {
		map_iterator_t it = _items_map.find(p.first);
		if (it != _items_map.end()) {
			switch (on_dup(it->second->second)) {
				case update:
					it->second->second = p.second;
				case leave:
					// The item is already there, leave it alone
					pthread_mutex_unlock(&this->_qmtx);
					return true;
				case renew:
				default:
					// The item is already there, move it to front
					_items_map.erase(it);
					this->_items_queue.erase(it->second);
					break;
			}
		}

		bool pushed = Queue_t::_push(p, timeout);

		if (pushed) {
			list_iterator_t first = this->_items_queue.begin();
			_items_map[p.first] = first;
		}

		return pushed;
	}

	bool _pop(const key_value_pair_t & p, double timeout) {
		bool popped = Queue_t::_pop(p, timeout);

		if (popped) {
			map_iterator_t it = _items_map.find(p.first);
			if (it != _items_map.end()) {
				_items_map.erase(it);
			}
		}

		return popped;
	}

	void _clear() {
		Queue_t::_clear();
		_items_map.clear();
	}

public:
	QueueSet(size_t limit=-1) : Queue_t(limit) {}

	size_t erase(const Key & key) {
		size_t items = 0;
		pthread_mutex_lock(&this->_qmtx);
		map_iterator_t it = _items_map.find(key);
		if (it != _items_map.end()) {
			this->_items_queue.erase(it->second);
			_items_map.erase(it);
			items++;
		}
		pthread_mutex_unlock(&this->_qmtx);
		return items;
	}

	bool push(const Key &key, const T & element, double timeout=-1.0) {
		key_value_pair_t p = key_value_pair_t(key, element);
		return push(p, timeout);
	}

#if T == Key
	bool push(const T & element, double timeout=-1.0) {
		key_value_pair_t p = key_value_pair_t(element, element);
		return Queue_t::push(p, timeout);
	}
#endif

	bool pop(T & element, double timeout=-1.0) {
		key_value_pair_t p;
		bool popped = Queue_t::_pop(p, timeout);
		if (popped) {
			element = p.second;
		}
		return popped;
	}
};

#endif /* XAPIAND_INCLUDED_QUEUE_H */
