#pragma once

#include <thread>
#include <mutex>
#include <condition_variable>
#include <exception>
#include <deque>
#include <vector>
#include <memory>
#include <functional>

namespace thmq{

/// Tracks the life of the Guard. 
typedef std::weak_ptr<void> Life_watcher;
typedef std::function<void()>  Msg_func;

class Guard{
public:
	Guard() : ptr_(std::make_shared<uint8_t>()) {}
	Guard(const Guard&) = delete;
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
	Message(Msg_func &&f, std::vector<Life_watcher> &&w) :
	  msg(std::move(f)), watch(std::move(w)) {}
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
	/// these methods are thread safe
	void add(Msg_func &&msg);
	void add(Msg_func &&msg, Life_watcher);
	void add(Msg_func &&msg, std::initializer_list<Life_watcher>);
	void add(Msg_func &&msg, std::vector<Life_watcher> &&);
	template<class T>
	void add(Msg_func &&msg, T &collection){
		std::vector<Life_watcher> lw(begin(collection), end(collection));
		add(std::move(msg), std::move(lw));
	}
	void add_front(Msg_func &&msg);
	void add_front(Msg_func &&msg, Life_watcher);
	void add_front(Msg_func &&msg, std::initializer_list<Life_watcher>);
	void add_front(Msg_func &&msg, std::vector<Life_watcher> &&);
	template<class T>
	void add_front(Msg_func &&msg, T &collection){
		std::vector<Life_watcher> lw(begin(collection), end(collection));
		add_front(std::move(msg), std::move(lw));
	}
	/// blocks till able to get the message. if message is got, but one of it's gurads is dead, then returns.
	/// this is thread safe as long as error_handler() is thread safe.
	void run_one() noexcept;
	/// runs loop over all the messages, untill stop() is called
	void run() noexcept;
	/// thread safe. does not block. all messages added before call to stop() will be processed by run().
	/// queue can be rerun after stop.
	void stop();

	bool empty() noexcept;
private:
	std::deque<Message> msgs_;
	std::mutex queue_mutex_;
	std::condition_variable wake_up_;
	std::function<void(std::exception_ptr)> eh_;
	bool stop_;
};



}
