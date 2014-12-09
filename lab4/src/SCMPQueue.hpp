#pragma once

#include <pthread.h>
#include "PThreadHelp.hpp"

namespace SCMP {

    using pthreadhelp::SimpleQueue;

    template<class T>
    class Queue : public SimpleQueue<T> {

    public:
        Queue();
        Queue(const Queue<T> &another);
        Queue<T>& operator=(const Queue<T> &another);
        ~Queue();

        void enqueue(const T &item);
        T dequeue();
        bool empty() const;
    private:
        using typename pthreadhelp::SimpleQueue<T>::QueueNode;
        using typename pthreadhelp::SimpleQueue<T>::PNode;

        PNode mStub;
    };

    template<class T>
    Queue<T>::Queue(const Queue<T> &another) {
        this->mBack = this->mFront = mStub = new QueueNode();
    }

    template<class T>
    Queue<T>& Queue<T>::operator=(const Queue<T> &another) {
        if (&another == this) {
            return *this;
        }
        this->mBack = this->mFront = mStub = new QueueNode();
        if (!another.empty()) {
            PNode cur = another.mFront;
            if (cur == another.mStub) {
                cur =  cur->next;
            }
            for (;cur; cur = cur->next) {
                enqueue(cur->data);
            }
        }
        return *this;
    }

    template<class T>
    Queue<T>::Queue() {
        this->mBack = this->mFront = mStub = new QueueNode();
    }

    template<class T>
    Queue<T>::~Queue() {}

    template<class T>
    bool Queue<T>::empty() const {
        return this->mBack == mStub;
    }

    template<class T>
    T Queue<T>::dequeue() {
        PNode front = this->mFront;
        PNode next = front->next;

        if (front == mStub) {
            // trying to pop en element from empty queue
            if (next == nullptr) {
                return T();
            }
            this->mFront = next;
            front = next;
            next = next->next;
        }

        if (next) {
            this->mFront = next;
            return front->data;
        }

        PNode back = this->mBack;
        // trying to pop en element from empty queue
        if (back != front) {
            return T();
        }

        mStub->next = nullptr;
        PNode prevNode = (QueueNode*) __sync_val_compare_and_swap(&this->mBack, this->mBack, mStub);
        prevNode->next = mStub;

        next = front->next;
        if (next) {
            this->mFront = next;
            return front->data;
        }

        return T();
    }

    template<class T>
    void Queue<T>::enqueue(T const &item) {
        PNode newNode = new QueueNode(item);
        PNode prevNode = (QueueNode*) __sync_val_compare_and_swap(&this->mBack, this->mBack, newNode);
        prevNode->next = this->mBack;
    }
}