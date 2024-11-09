/*
** Builds with >= c++11: c++ <FileName.cxx> -std=c++11 
**$Log$
*/
#include <iostream>
#include <thread>
#include <vector>
#include <mutex>

std::mutex mtx;
std::vector<int> sharedData;

void appendToVector(int value) {
    // Lock the mutex to protect the shared vector
    std::lock_guard<std::mutex> lock(mtx);
    sharedData.push_back(value);
    std::cout << "Thread added: " << value << std::endl;
}

int main() {
    std::thread t1(appendToVector, 1);
    std::thread t2(appendToVector, 2);
    std::thread t3(appendToVector, 3);

    t1.join();
    t2.join();
    t3.join();

    // Print the final state of the shared vector
    std::cout << "Final vector content: ";
    for (int value : sharedData) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    return 0;
}
