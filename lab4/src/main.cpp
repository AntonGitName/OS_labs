#include <iostream>
#include <vector>
#include "SCMPQueue.hpp"
#include "MCMPQueue.hpp"
#include <utility>

const int numTests = 1000;

MCMP::Queue<int> queueMCMP;
SCMP::Queue<int> queueSCMP;

double consumerTime;
double producerTime;

void clearQueues() {
    queueMCMP = MCMP::Queue<int>();
    queueSCMP = SCMP::Queue<int>();
    consumerTime = producerTime = 0.0;
}

typedef void *(action_t)(void *);

void *produceMCMP(void *arg) {
    int t = *(int*)(&arg);
    clock_t begin = clock();
    for (int i = 0; i < numTests; ++i) {
        queueMCMP.enqueue(t);
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    producerTime += elapsed_secs;
    return nullptr;
}

void *produceSCMP(void *arg) {
    int t = *(int*)(&arg);
    clock_t begin = clock();
    for (int i = 0; i < numTests; ++i) {
        queueSCMP.enqueue(t);
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    producerTime += elapsed_secs;
    return nullptr;
}

void *consumeMCMP(void *arg) {
    int t = *(int*)(&arg);
    clock_t begin = clock();
    for (int i = 0; i < numTests; ++i) {
        queueMCMP.dequeue();
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    consumerTime += elapsed_secs;
    return nullptr;
}

void *consumeSCMP(void *arg) {
    int t = *(int*)(&arg);
    clock_t begin = clock();
    for (int i = 0; i < numTests; ++i) {
        queueSCMP.dequeue();
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    consumerTime += elapsed_secs;
    return nullptr;
}

auto test(size_t consumers, action_t consumerAction, size_t producers, action_t producerAction) {
    clearQueues();
    std::vector<pthread_t> threads(consumers + producers);

    for (size_t i = 0; i < producers; ++i) {
        pthread_create(&threads[i], nullptr, producerAction, (void*)(i));
    }
    for (size_t i = producers; i < threads.size(); ++i) {
        pthread_create(&threads[i], nullptr, consumerAction, (void*)(i));
    }
    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }

    return std::make_pair(consumerTime / consumers, producerTime / producers);
}

void testMCMP(size_t consumers, size_t producers) {
    printf("MCMP queue test start\n");
    auto result = test(consumers, consumeMCMP, producers, produceMCMP);
    printf("Average consumer %F, Average producer %F\n", result.first, result.second);
    printf("MCMP queue test end\n");
}

void testSCMP(size_t producers) {
    printf("SCMP queue test start\n");
    clearQueues();
    size_t consumers = 1;
    auto result = test(consumers, consumeSCMP, producers, produceSCMP);
    printf("Average consumer %F, Average producer %F\n", result.first, result.second);
    printf("SCMP queue test end.\n");
}

void testBoth(size_t producers) {
    printf("Both queues test start\n");
    clearQueues();
    size_t consumers = 1;
    auto resultSCMP = test(consumers, consumeSCMP, producers, produceSCMP);
    auto resultMCMP = test(consumers, consumeMCMP, producers, produceMCMP);
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