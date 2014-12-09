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

    protected:
        struct QueueNode;
        typedef QueueNode *PNode;

        struct QueueNode {
            T data;
            PNode next;

            QueueNode(T data = T(), PNode next = nullptr) : data(data), next(next) {
            }
        };

        PNode mFront;
        PNode volatile mBack;
    };

    template<class T>
    SimpleQueue<T>::SimpleQueue(const SimpleQueue<T> &another)
            : mFront(nullptr), mBack(nullptr) {
        mBack = new QueueNode();
        mFront = new QueueNode(T(), mBack);
        if (!another.empty()) {
            for (PNode cur = another.mFront->next; cur; cur = cur->next) {
                enqueue(cur->data);
            }
        }
    }

    template<class T>
    SimpleQueue<T>& SimpleQueue<T>::operator=(const SimpleQueue<T> &other) {
        if (&other == this) {
            return *this;
        }
        mBack = new QueueNode();
        mFront = new QueueNode(T(), mBack);
        if (!other.empty()) {
            for (PNode cur = other.mFront->next; cur != other.mBack; cur = cur->next) {
                enqueue(cur->data);
            }
        }
        return *this;
    }

    template<class T>
    SimpleQueue<T>::SimpleQueue()
            : mFront(nullptr), mBack(nullptr) {
        mBack = new QueueNode();
        mFront = new QueueNode(T(), mBack);
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
        return mFront->next == mBack;
    }

    template<class T>
    T SimpleQueue<T>::dequeue() {
        T result = mFront->next->data;
        PNode tmp = mFront;
        mFront = mFront->next;
        delete tmp;
        return result;
    }

    template<class T>
    void SimpleQueue<T>::enqueue(T const &item) {
        mBack->next = new QueueNode();
        mBack->data = item;
        mBack = mBack->next;
    }
}