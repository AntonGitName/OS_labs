#pragma once

#include <pthread.h>

namespace SCMP {

    template<class T>
    class Queue {

    public:
        Queue();

        Queue(const Queue<T> &another);

        ~Queue();

        void enqueue(const T &item);

        T dequeue();

        bool empty() const;

    private:
        struct QueueNode;
        typedef QueueNode *PNode;

        struct QueueNode {
            T data;
            PNode next;
            PNode prev;

            QueueNode(T data, PNode next = nullptr, PNode prev = nullptr) : data(data), next(next), prev(prev) {
            }
        };

        size_t mSize;
        PNode mFront;
        PNode mBack;

    };

    template<class T>
    Queue<T>::Queue(const Queue<T> &another)
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
    Queue<T>::Queue()
            : mSize(0), mFront(nullptr), mBack(nullptr) {
    }

    template<class T>
    Queue<T>::~Queue() {
        for (PNode cur = mFront, tmp; cur; cur = tmp) {
            tmp = cur->next;
            delete cur;
        }
    }

    template<class T>
    bool Queue<T>::empty() const {
        return mFront == nullptr;
    }

    template<class T>
    T Queue<T>::dequeue() {
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
    void Queue<T>::enqueue(T const &item) {
        if (mFront != nullptr) {
            mBack = mBack->next = new QueueNode(item, nullptr, mBack);
        } else {
            mFront = mBack = new QueueNode(item);
        }
        ++mSize;
    }
}