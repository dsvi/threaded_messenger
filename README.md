# A c++11 library for multithreaded message passing #

### Usage ###

Create message queue and run it:

    Message_queue mq;
    mq.run();

Than, from another thread, add some code you'd like to be executed in the running thread.
  
    mq.add([]{do_some_stuff_in_another_thread();});

If you would like to do something on an object from another thread, but are not sure if it's going to be still alive, just pass a weak pointer to it. If object is dead, message isn't going to be executed.

    weak_ptr<Class> wp_obj;
    Class *obj; // <- this actually lives in another thread and its life is controlled **there** by a shared_ptr
    mq.add([]{obj->do_some_stuff_in_another_thread();}, {wp_obj, another_wp});

Imagine you'd like to execute a group of messages, probably in a thread pool, and wait for them

    Group g;
    mq.add(g.add([]{do_some_stuff_in_thread_pool_1();}));
    mq.add(g.add([]{do_some_stuff_in_thread_pool_2();}));
    mq.add(g.add([]{do_some_stuff_in_thread_pool_3();}));
    //...
    g.wait(); // <- blocks till all the messages in the group either done or failed
    
For more advanced usage see [tests.](test.c++)

### License ###

The person who associated a work with this deed has dedicated the work to the public domain by waiving all of his or her rights to the work worldwide under copyright law, including all related and neighboring rights, to the extent allowed by law.

You can copy, modify, distribute and perform the work, even for commercial purposes, all without asking permission.
