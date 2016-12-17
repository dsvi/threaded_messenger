#include <thread>
#include "lest.hpp"
#include "message_queue.h"
#include "group.h"

using namespace std;
using namespace thmq;

const lest::test specification[] =
{
  CASE( "Basic multithreaded test" )
  {
    int count = 0;
    Message_queue mq;
    const uint tries = 1000000;
    thread mqt(&Message_queue::run, &mq);
    thread t([&]{
      for (uint i=0; i < tries; i++)
        mq.add([&]{count--;});
    });
    for (uint i=0; i < tries+2; i++)
      mq.add([&]{count++;});
    t.join();
    mq.stop();
    mqt.join();
    EXPECT( count == 2 );
  },

  CASE( "Error reporting test" )
  {
    int count = 0;
    Message_queue mq;
    mq.error_handler([&](exception_ptr eptr){
      try {
        if (eptr) {
          std::rethrow_exception(eptr);
        }
      } catch(const std::runtime_error& e) {
        count++;;
      }
    });
    thread t(&Message_queue::run, &mq);
    mq.add([&]{
      count++;
      throw runtime_error("");
    });
    mq.stop();
    t.join();
    EXPECT( count == 2 );
  },

  CASE( "Guards test" )
  {
    int count = 0;
    Message_queue mq;
    thread t(&Message_queue::run, &mq);
    unique_ptr<Guard> gp(new Guard());
    auto lw = gp->watcher();
    mq.add([&](){count++;}, {lw});
    mq.add([&](){count++;}, lw);
    mq.add([&](){gp.reset();}, lw); // guard dead. messages wont be executed
    mq.add([&](){count++;}, {lw});
    mq.add([&](){count++;}, lw);
    mq.stop();
    t.join();
    EXPECT( count == 2 );
  },
  CASE( "Group test" )
  {
    int count = 0;
    Message_queue mq;
    const uint tries = 1000000;
    vector<thread> threads;
    bool threads_to_quit = false;
    size_t num_threads = 10;
    for ( size_t i = 0; i < num_threads; i--){
      threads.emplace_back([&]{
        while(!threads_to_quit)
          mq.run_one();
      });
    }
    Group g;
    for (uint i=0; i < tries; i++)
      mq.add(g.add([&]{count--;}));
    for (uint i=0; i < tries+2; i++)
      mq.add(g.add([&]{count++;}));
    g.wait();
    EXPECT( count == 2 );
    threads_to_quit = true;
    for ( size_t i = 0; i < num_threads; i--){
      mq.add([&]{;});
    }
    for ( size_t i = 0; i < num_threads; i--){
      threads[i].join();
    }
  },

};

int main( int argc, char * argv[] )
{
	return lest::run( specification, argc, argv );
}
