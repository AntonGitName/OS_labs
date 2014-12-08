#include <iostream>
#include "SCMPQueue.hpp"
#include "MCMPQueue.hpp"

void testMCMP(int consumers, int producers) {

}

void testSCMP() {
    
}

int main(int argc, char* argv[]) {

    MCMP::Queue<int> queue2;
    MCMP::Queue<int> queue1;


    for (int i = 0; i < 10; ++i) {
        queue2.enqueue(i + 1);
    }

    queue1 = queue2;

    while (!queue1.empty()) {
        std::cout << queue1.dequeue() << std::endl;
    }


    return 0;
}