#pragma once

#include <pthread.h>
#include "PThreadHelp.hpp"

namespace MCMP {

    using pthreadhelp::SimpleQueue;

    class EmptyQueueException : public std::exception {
    public:
        const char *what() const throw() {
            return exception::what();
        }
    };

    template<class T>
    class Queue : public SimpleQueue<T> {

    public:
        Queue();
        Queue(const Queue<T> &another);
        Queue<T>& operator=(const Queue<T> &another);
        ~Queue();

        void enqueue(const T &item);
        T dequeue();
        T forceDequeue();
        bool empty() const;

    private:
        mutable pthread_mutex_t mutex;
        pthread_cond_t notEmptyCond;

    };

    template<class T>
    Queue<T>::Queue(const Queue<T> &another) : SimpleQueue<T>::SimpleQueue(another) {
        pthread_cond_init(&notEmptyCond, NULL);
        pthread_mutex_init(&mutex, NULL);
    }

    template<class T>
    Queue<T>& Queue<T>::operator=(const Queue<T> &another) {
        pthreadhelp::LockGuard lg(mutex);
        this->SimpleQueue<T>::operator=(another);
        return *this;
    }

    template<class T>
    Queue<T>::Queue() : SimpleQueue<T>::SimpleQueue() {
        pthread_cond_init(&notEmptyCond, NULL);
        pthread_mutex_init(&mutex, NULL);
    }

    template<class T>
    Queue<T>::~Queue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&notEmptyCond);
    }

    template<class T>
    bool Queue<T>::empty() const {
        pthreadhelp::LockGuard lg(mutex);
        return SimpleQueue<T>::empty();
    }

    // wait and pop
    template<class T>
    T Queue<T>::dequeue() {
        pthreadhelp::LockGuard lg(mutex);
        while (SimpleQueue<T>::empty()) {
            pthread_cond_wait(&notEmptyCond, &mutex);
        }
        return SimpleQueue<T>::dequeue();
    }

    // simple pop
    template<class T>
    T Queue<T>::forceDequeue() {
        pthreadhelp::LockGuard lg(mutex);
        if (SimpleQueue<T>::empty()) {
            throw EmptyQueueException();
        }
        return SimpleQueue<T>::dequeue();
    }

    template<class T>
    void Queue<T>::enqueue(T const &item) {
        pthreadhelp::LockGuard lg(mutex);
        SimpleQueue<T>::enqueue(item);
        pthread_cond_signal(&notEmptyCond);
    }
}