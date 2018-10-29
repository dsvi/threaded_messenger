#pragma once

#include <memory>
#include <condition_variable>
#include <functional>

namespace thmq{

#include "noncopyable.h"

/**
Creates a group of messages, that could be added to Message_queue (which is possibly processed by several threads at a time in a Thread_pool),
and then allaws to wait() for their execution to complete.
usage:
\code
Message_queue mq;
Group g;
mq.add( g.add( []{} ));
mq.add( g.add( []{} ));
mq.add( g.add( []{} ));
// ...
g.wait();
// at this point all the added messages have finished execution
\endcode
*/
class Group : noncopyable
{
public:
	Group();
	std::function<void()> add(std::function<void()> &&f);
	void wait();
private:
	struct Blocker{
		std::mutex mtx;
		std::condition_variable cv;
		unsigned int members_count{0};
	};
	std::shared_ptr<Blocker> pending_tasks_;
};

}
