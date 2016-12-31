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
	decltype(pending_tasks_) s;
	{
		unique_lock<mutex> lk(pending_tasks_->mtx);
		s = pending_tasks_;
		s->can_go = false;
	}
	return [fun = move(f), s]{
		At_exit([&]{
			unique_lock<mutex> lk(s->mtx);
			if (s.use_count() == 2){
				s->can_go = true;
				lk.unlock();
				s->cv.notify_all();
			}
		});
		fun();
	};
}

void Group::wait()
{
	unique_lock<mutex> lk(pending_tasks_->mtx);
	while(!pending_tasks_->can_go)
		pending_tasks_->cv.wait(lk);
}

}
