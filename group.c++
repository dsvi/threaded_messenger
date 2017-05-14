#include <mutex>
#include "group.h"

using namespace std;

namespace thmq{

class At_exit
{
public:
	explicit At_exit(std::function<void()> &&f): f_(std::move(f)) {}
	~At_exit() { f_(); }
private:
	std::function<void()> f_;
};

Group::Group() : pending_tasks_{ make_shared<Blocker>() }
{
}

std::function<void()> Group::add(std::function<void()> &&f)
{
	{
		unique_lock<mutex> lk(pending_tasks_->mtx);
		pending_tasks_->members_count++;
	}
	return [fun = move(f), s = pending_tasks_]{
		At_exit guard([&]{
			unique_lock<mutex> lk(s->mtx);
			s->members_count--;
			lk.unlock();
			s->cv.notify_all();
		});
		fun();
	};
}

void Group::wait()
{
	unique_lock<mutex> lk(pending_tasks_->mtx);
	while(pending_tasks_->members_count)
		pending_tasks_->cv.wait(lk);
}

}
