#include "../include/ThreadPool.h"
#include <iostream>

using std::thread;
using std::cout;
using std::endl;

ThreadPool::ThreadPool(size_t threadNum, size_t queSize)
: _threadNum(threadNum)
, _threads(threadNum)
, _queSize(queSize)
, _taskQue(_queSize)
{}

ThreadPool::~ThreadPool() {}

void ThreadPool::start() {
    for (size_t i = 0; i < _threadNum; i++) {
        _threads[i] = thread(&ThreadPool::doTask, this);
    }
}

void ThreadPool::stop() {
    while (!_taskQue.empty()) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    for (size_t i = 0; i < _threadNum; ++i) {
        _taskQue.push(nullptr);
    }

    for (auto& ele : _threads) {
        ele.join();
    }
}

void ThreadPool::addTask(Task&& pTask) {
    if (pTask) {
        _taskQue.push(std::move(pTask));
    }
}

Task ThreadPool::getTask() {
    return _taskQue.pop();
}

void ThreadPool::doTask() {
    while (1) {
        Task Task = getTask();
        if (Task) {
            Task();
        } else {
            cout << "thread id " << std::this_thread::get_id() << "  exiting!" << endl;
            break;
        }
    }
}
