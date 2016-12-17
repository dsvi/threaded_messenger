#include "message_queue.h"

using namespace std;

namespace thmq{


Message_queue::Message_queue(){}

void Message_queue::error_handler(std::function<void (std::exception_ptr)> &&eh)
{
	eh_ = move(eh);
}

void Message_queue::add(Msg_func &&f)
{
	add(move(f), initializer_list<Life_watcher>());
}

void Message_queue::add(Msg_func &&msg, Life_watcher lw)
{
	add(move(msg), {lw});
}

void Message_queue::add(Msg_func &&f, std::initializer_list<Life_watcher> grds)
{
	{
		unique_lock<mutex> lock(queue_mutex_);
		msgs_.emplace(move(f), grds);
	}
	wake_up_.notify_one();
}

void Message_queue::run_one() noexcept
{
	try{
		std::unique_lock<std::mutex> lock(queue_mutex_);
		while(msgs_.empty())
			wake_up_.wait(lock);

		Message msg = move(msgs_.front());
		msgs_.pop();
		lock.unlock();
		for (auto g: msg.watch)
			if (g.expired())
				return;
		msg.msg();
	}
	catch(...){
		if (eh_)
			eh_(current_exception());
	}
}

void Message_queue::run() noexcept
{
	stop_ = false;
	do
	{
		run_one();
	}
	while(!stop_);
}

void Message_queue::stop()
{
	add([&](){
		stop_ = true;
	});
}

}
