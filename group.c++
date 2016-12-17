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
	pending_tasks_->can_go = false;
	return [fun = move(f), s = pending_tasks_]{
		At_exit([&]{
			if (s.use_count() == 2){
				s->can_go = true;
				s->cv.notify_all();
			}
		});
		fun();
	};
}

void Group::wait()
{
	mutex cvm;
	unique_lock<mutex> lk(cvm);
	pending_tasks_->cv.wait(lk, [&]{return pending_tasks_->can_go;});
}

}
