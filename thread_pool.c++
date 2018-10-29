#include "thread_pool.h"

namespace  thmq {

using namespace std;

Thread_pool::Thread_pool()
{
	time_to_quit_ = false;
	int num_threads = thread::hardware_concurrency();
	do{
		thread_pool_.emplace_back([this]{
			while(!time_to_quit_)
				queue_.run_one();
		});
	}
	while(--num_threads > 0);
}

Thread_pool::~Thread_pool()
{
	time_to_quit_ = true;
	for (int i = thread_pool_.size(); i--;)
		queue_.add([]{});
	for (auto &t : thread_pool_)
		t.join();
}

}
