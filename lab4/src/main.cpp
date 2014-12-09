#include <iostream>
#include <vector>
#include "SCMPQueue.hpp"
#include "MCMPQueue.hpp"

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

void *produceMCMP(void *arg) {
    int t = *(int*)(&arg);
    clock_t begin = clock();
    for (int i = 0; i < numTests; ++i) {
        queueMCMP.enqueue(t);
    }
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    printf("Thread #%d (producer) was executed for %F seconds\n", t, elapsed_secs);
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
    printf("Thread #%d (producer) was executed for %F seconds\n", t, elapsed_secs);
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
    printf("Thread #%d (consumer) was executed for %F seconds\n", t, elapsed_secs);
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
    printf("Thread #%d (consumer) was executed for %F seconds\n", t, elapsed_secs);
    consumerTime += elapsed_secs;
    return nullptr;
}

void testMCMP(int consumers, int producers) {
    printf("MCMP queue test start\n");
    clearQueues();
    std::vector<pthread_t> threads(consumers + producers);

    for (size_t i = 0; i < producers; ++i) {
        pthread_create(&threads[i], nullptr, produceMCMP, (void*)(i));
    }

    for (size_t i = producers; i < threads.size(); ++i) {
        pthread_create(&threads[i], nullptr, consumeMCMP, (void*)(i));
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }
    printf("MCMP queue test end\n");
    printf("Average consumer %F, Average producer %F\n", consumerTime / consumers, producerTime / producers);
}

void testSCMP(int producers) {
    printf("SCMP queue test start\n");
    clearQueues();
    int consumers = 1;
    std::vector<pthread_t> threads(consumers + producers);

    for (size_t i = 0; i < producers; ++i) {
        pthread_create(&threads[i], nullptr, produceSCMP, (void*)(i));
    }

    for (size_t i = producers; i < threads.size(); ++i) {
        pthread_create(&threads[i], nullptr, consumeSCMP, (void*)(i));
    }

    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }
    printf("SCMP queue test end.\n");
    printf("Average consumer %F, Average producer %F\n", consumerTime / consumers, producerTime / producers);
}

void testBoth(int producers) {
    printf("Both queues test start\n");
    clearQueues();
    int consumers = 1;
    std::vector<pthread_t> threads(consumers + producers);

    for (size_t i = 0; i < producers; ++i) {
        pthread_create(&threads[i], nullptr, produceSCMP, (void*)(i));
    }
    for (size_t i = producers; i < threads.size(); ++i) {
        pthread_create(&threads[i], nullptr, consumeSCMP, (void*)(i));
    }
    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }

    double SCMPconsumer = consumerTime / consumers;
    double SCMPproducer = producerTime / producers;

    clearQueues();
    for (size_t i = 0; i < producers; ++i) {
        pthread_create(&threads[i], nullptr, produceMCMP, (void*)(i));
    }
    for (size_t i = producers; i < threads.size(); ++i) {
        pthread_create(&threads[i], nullptr, consumeMCMP, (void*)(i));
    }
    for (size_t i = 0; i < threads.size(); ++i) {
        pthread_join(threads[i], nullptr);
    }

    printf("Both queues test end.\n");
    printf("MCMP: Average consumer %F, Average producer %F\n", consumerTime / consumers, producerTime / producers);
    printf("SCMP: Average consumer %F, Average producer %F\n", SCMPconsumer, SCMPproducer);

}

int main(int argc, char* argv[]) {

    if (argc != 3) {
        printf("Usage:\ttest c p, where c is number of consumers and p is number of producers\n\n");
        printf("Example:\ttest 5 3\n");
        printf("\t\t This will start 3 tests:\n");
        printf("\t\t 1. SCMP queue test with 1 consumer and 3 producers\n");
        printf("\t\t 2. MCMP queue test with 5 consumers and 3 producers\n");
        printf("\t\t 3. Both queues test with 1 consumer and 3 producers\n");
    }
    int c = atoi(argv[1]);
    int p = atoi(argv[2]);

    testMCMP(c, p);
    testSCMP(p);
    testBoth(p);

    return 0;
}