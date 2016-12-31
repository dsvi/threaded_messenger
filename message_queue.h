#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <deque>
#include <vector>
#include <memory>

namespace thmq{

typedef std::weak_ptr<void> Life_watcher;
typedef std::function<void()>  Msg_func;
class Guard{
public:
	Guard() : ptr_(std::make_shared<uint8_t>()) {}
	Life_watcher watcher() const{
		return ptr_;
	}
private:
	std::shared_ptr<void> ptr_;
};


/// \cond
struct Message{
	Message(Msg_func &&f, std::initializer_list<Life_watcher> w) :
	  msg(std::move(f)), watch(w) {}
	Msg_func           msg;
	std::vector<Life_watcher> watch;
};
/// \endcond

#include "noncopyable.h"

/// executes \e messages in the added order
class Message_queue : public noncopyable
{
public:
	Message_queue();
	Message_queue(Message_queue&&) =default;
	/// exceptions from messages are cought and \p eh is called
	/// by default they are just cought and nothing happens.
	void error_handler(std::function<void(std::exception_ptr)> &&eh);
	/// this method is thread safe
	void add(Msg_func &&msg);
	void add(Msg_func &&msg, Life_watcher);
	void add(Msg_func &&msg, std::initializer_list<Life_watcher>);
	void add_front(Msg_func &&msg);
	void add_front(Msg_func &&msg, Life_watcher);
	void add_front(Msg_func &&msg, std::initializer_list<Life_watcher>);
	/// blocks till able to get the message. if message is got, but one of it's gurads is dead, then returns.
	/// this is thread safe as long as error_handler() is thread safe.
	void run_one() noexcept;

	/// runs loop over all the messages, untill stop() is called
	void run() noexcept;
	/// thread safe. does not block. all messages added before call to stop() will be processed by run().
	/// queue can be rerun after stop.
	void stop();
private:
	std::deque<Message> msgs_;
	std::mutex queue_mutex_;
	std::condition_variable wake_up_;
	std::function<void(std::exception_ptr)> eh_;
	bool stop_;
};



}
