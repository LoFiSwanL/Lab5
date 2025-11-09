#include <iostream>
#include <thread>
#include <future>
#include <chrono>
#include <syncstream>
#include <string>

using namespace std;

void do_work(const string &name, int seconds){
    this_thread::sleep_for(chrono::seconds(seconds));
    osyncstream(cout) << "done (" << seconds << "s)" << endl;
}

void quick(const string &name){
    do_work(name, 1);
}

void slow(const string &name){
    do_work(name, 7);
}

void work(){
    auto start_time = chrono::steady_clock::now();

    promise<void> c2_promise, d2_promise;
    future<void> c2_future = c2_promise.get_future();
    future<void> d2_future = d2_promise.get_future();

    auto worker = async(launch::async, [&](){
        slow("A");
        slow("B");
        quick("C1");
        c2_future.wait();
        quick("D1");
        d2_future.wait();
        quick("F");
    });

    quick("C2");
    c2_promise.set_value();

    quick("D2");
    d2_promise.set_value();

    worker.get();

    auto end_time = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(end_time - start_time).count();

    osyncstream(cout) << "Work is done! \nTotal time: " << duration << " s" << endl;
}

int main(){
    cout << "Start..." << endl;
    work();

    return 0;
}