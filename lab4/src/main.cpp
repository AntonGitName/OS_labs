#include <iostream>
#include <vector>
#include "SCMPQueue.hpp"
#include "MCMPQueue.hpp"
#include <utility>

const int numTests = 1000;

typedef void *(action_t)(void *);
typedef pthreadhelp::SimpleQueue<int> TestQueue;
typedef std::pair<TestQueue*, int> arg_t;

MCMP::Queue<int> queueMCMP;
SCMP::Queue<int> queueSCMP;

double consumerTime;
double producerTime;

void clearQueues() {
    queueMCMP = MCMP::Queue<int>();
    queueSCMP = SCMP::Queue<int>();
    consumerTime = producerTime = 0.0;
}

void *produce(void *arg) {
    arg_t *Arg = (arg_t*)arg;
    int threadNum = Arg->second;
    TestQueue *pQueue = Arg->first;
    clock_t begin = clock();
    for (int i = 0; i < numTests; ++i) {
        pQueue->enqueue(threadNum);
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    producerTime += elapsed_secs;
    return nullptr;
}

void *consume(void *arg) {
    arg_t *Arg = (arg_t*)arg;
    TestQueue *pQueue = Arg->first;
    clock_t begin = clock();
    for (int i = 0; i < numTests; ++i) {
        pQueue->dequeue();
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    consumerTime += elapsed_secs;
    return nullptr;
}

auto test(TestQueue *Queue, size_t consumers, size_t producers) {
    clearQueues();
    std::vector<pthread_t> threads(consumers + producers);
    std::vector<arg_t*> arguments(consumers + producers);

    for (size_t i = 0; i < producers; ++i) {
        arguments[i] = new arg_t(Queue, i);
        pthread_create(&threads[i], nullptr, produce, arguments[i]);
    }
    for (size_t i = producers; i < threads.size(); ++i) {
        arguments[i] = new arg_t(Queue, i);
        pthread_create(&threads[i], nullptr, consume, arguments[i]);
    }
    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], nullptr);
        delete arguments[i];
    }

    return std::make_pair(consumerTime / consumers, producerTime / producers);
}

void testMCMP(size_t consumers, size_t producers) {
    printf("MCMP queue test start\n");
    auto result = test(&queueMCMP, consumers, producers);
    printf("Average consumer %F, Average producer %F\n", result.first, result.second);
    printf("MCMP queue test end\n");
}

void testSCMP(size_t producers) {
    printf("SCMP queue test start\n");
    clearQueues();
    size_t consumers = 1;
    auto result = test(&queueSCMP, consumers, producers);
    printf("Average consumer %F, Average producer %F\n", result.first, result.second);
    printf("SCMP queue test end.\n");
}

void testBoth(size_t producers) {
    printf("Both queues test start\n");
    clearQueues();
    size_t consumers = 1;
    auto resultSCMP = test(&queueSCMP, consumers, producers);
    auto resultMCMP = test(&queueMCMP, consumers, producers);
    printf("MCMP: Average consumer %F, Average producer %F\n", resultMCMP.first, resultMCMP.second);
    printf("SCMP: Average consumer %F, Average producer %F\n", resultSCMP.first, resultSCMP.second);
    printf("Both queues test end.\n");
}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage:  \tlab4 c p\n\t\t\t where c is number of consumers and p is number of producers\n\n");
        printf("Example:\tlab4 5 3\n");
        printf("\t\t This will start 3 tests:\n");
        printf("\t\t 1. SCMP queue test with 1 consumer and 3 producers\n");
        printf("\t\t 2. MCMP queue test with 5 consumers and 3 producers\n");
        printf("\t\t 3. Both queues test with 1 consumer and 3 producers\n");
        return 0;
    }
    int c = atoi(argv[1]);
    int p = atoi(argv[2]);

    testMCMP(c, p);
    testSCMP(p);
    testBoth(p);

    return 0;
}