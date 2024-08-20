#include "../include/TaskQueue.h"

TaskQueue::TaskQueue(size_t capacity) 
: _capacity(capacity)
{}

TaskQueue::~TaskQueue() {}

void TaskQueue::push(ElemType &&ptask) {
    unique_lock<mutex> ul(_mutex);
    if (full()) {
        _notFull.wait(ul);
    }
    _que.emplace(std::move(ptask));
    _notEmpty.notify_one();
}

ElemType TaskQueue::pop() {
    unique_lock<mutex> ul(_mutex);
    while (empty()) {
        _notEmpty.wait(ul);
    }

    if (!empty()) {
        ElemType tmp = _que.front();
        _que.pop();
        _notFull.notify_one();
        return tmp;
    }
    return nullptr;
}

bool TaskQueue::empty() const
{
    return 0 == _que.size();
}

bool TaskQueue::full() const
{
    return _capacity == _que.size();
}
