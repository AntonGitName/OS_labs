#pragma once

#include <pthread.h>

namespace pthreadhelp {

    class LockGuard {
    public:
        LockGuard() = delete ;
        LockGuard(pthread_mutex_t& mutex) : mutex(mutex) {
            pthread_mutex_lock(&mutex);
        }
        ~LockGuard() {
            pthread_mutex_unlock(&mutex);
        }
    private:
        pthread_mutex_t& mutex;
    };

    template<class T>
    class SimpleQueue {

    public:
        SimpleQueue();
        SimpleQueue(const SimpleQueue<T> &another);
        SimpleQueue<T>& operator=(const SimpleQueue<T> &other);
        virtual ~SimpleQueue();

        virtual void enqueue(const T &item);
        virtual T dequeue();
        virtual bool empty() const;

    private:
        struct QueueNode;
        typedef QueueNode *PNode;

        struct QueueNode {
            T data;
            PNode prev;
            PNode next;

            QueueNode(T data, PNode prev = nullptr, PNode next = nullptr) : data(data), prev(prev), next(next) {
            }
        };

        size_t mSize;
        PNode mFront;
        PNode mBack;
    };

    template<class T>
    SimpleQueue<T>::SimpleQueue(const SimpleQueue<T> &another)
            : mSize(0), mFront(nullptr), mBack(nullptr) {
        mSize = another.mSize;
        if (mSize) {
            PNode newNode = new QueueNode(another.mFront->data);
            mFront = newNode;
            for (PNode cur = another.mFront->next; cur; cur = cur->next) {
                newNode = newNode->next = new QueueNode(cur->data, newNode);
            }
            mBack = newNode;
        }
    }

    template<class T>
    SimpleQueue<T>& SimpleQueue<T>::operator=(const SimpleQueue<T> &other) {
        if (&other == this) {
            return *this;
        }
        mSize = other.mSize;
        mFront = mBack = nullptr;
        if (mSize) {
            PNode newNode = new QueueNode(other.mFront->data);
            mFront = newNode;
            for (PNode cur = other.mFront->next; cur; cur = cur->next) {
                newNode = newNode->next = new QueueNode(cur->data, newNode);
            }
            mBack = newNode;
        }
        return *this;
    }

    template<class T>
    SimpleQueue<T>::SimpleQueue()
            : mSize(0), mFront(nullptr), mBack(nullptr) {
    }

    template<class T>
    SimpleQueue<T>::~SimpleQueue() {
        for (PNode cur = mFront, tmp; cur; cur = tmp) {
            tmp = cur->next;
            delete cur;
        }
    }

    template<class T>
    bool SimpleQueue<T>::empty() const {
        return mFront == nullptr;
    }

    template<class T>
    T SimpleQueue<T>::dequeue() {
        T result = mFront->data;
        --mSize;
        PNode tmp = mFront;
        mFront = mFront->next;
        if (mFront == nullptr) {
            mBack = nullptr;
        }
        delete tmp;
        return result;
    }

    template<class T>
    void SimpleQueue<T>::enqueue(T const &item) {
        if (mFront != nullptr) {
            mBack = mBack->next = new QueueNode(item, mBack);
        } else {
            mFront = mBack = new QueueNode(item);
        }
        ++mSize;
    }
}