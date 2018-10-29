#pragma once

#include "message_queue.h"

namespace  thmq {

/**
 * Executes messages from the queue() in a pool of threads, the size of std::thread::hardware_concurrency()
 */
class Thread_pool
{
public:
	Thread_pool();
	Thread_pool(Thread_pool &&) = default;
	~Thread_pool();

	Message_queue& queue(){
		return queue_;
	}
private:
	Message_queue queue_;
	std::vector<std::thread> thread_pool_;
	bool time_to_quit_;
};

}
