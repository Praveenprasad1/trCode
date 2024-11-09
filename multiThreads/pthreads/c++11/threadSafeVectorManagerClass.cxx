/*
** Build with c++11 or above: c++ --std=c++11 <thisFile>
**$Log$
*/
#include <iostream>
#include <vector>
#include <mutex>
#include <thread>

class VectorManager {
private:
    std::vector<int> vec;   // Encapsulated vector
    std::mutex mtx;         // Mutex for thread safety

public:
    // Add an element to the vector (push_back)
    void push(int value) {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        vec.push_back(value);
        std::cout << "Added: " << value << std::endl;
    }

    // Remove the last element (pop_back)
    void pop() {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        if (!vec.empty()) {
            int removedValue = vec.back();
            vec.pop_back();
            std::cout << "Removed: " << removedValue << std::endl;
        } else {
            std::cout << "Vector is empty, cannot pop." << std::endl;
        }
    }

    // Get the last element (back)
    int getLast() {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        if (!vec.empty()) {
            return vec.back();
        } else {
            std::cerr << "Vector is empty, no elements to return." << std::endl;
            return -1; // Returning -1 to signify error
        }
    }

    // Access an element at a specific index (operator[])
    int getElement(size_t index) {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        if (index < vec.size()) {
            return vec[index];
        } else {
            std::cerr << "Index out of bounds." << std::endl;
            return -1; // Return -1 to signify error
        }
    }

    // Print the contents of the vector
    void print() {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        std::cout << "Vector contents: ";
        for (int val : vec) {
            std::cout << val << " ";
        }
        std::cout << std::endl;
    }

    // Get the size of the vector
    size_t size() {
        std::lock_guard<std::mutex> lock(mtx); // Lock the mutex
        return vec.size();
    }
};

void addElements(VectorManager &manager, int start, int count) {
    for (int i = start; i < start + count; ++i) {
        manager.push(i);
    }
}

void removeElements(VectorManager &manager, int count) {
    for (int i = 0; i < count; ++i) {
        manager.pop();
    }
}

int main() {
    VectorManager manager;

    // Create threads to add and remove elements
    std::thread t1(addElements, std::ref(manager), 1, 5); // Add elements 1 to 5
    std::thread t2(addElements, std::ref(manager), 6, 5); // Add elements 6 to 10
    std::thread t3(removeElements, std::ref(manager), 3); // Remove 3 elements

    t1.join();
    t2.join();
    t3.join();

    // Print final vector contents
    manager.print();

    return 0;
}
