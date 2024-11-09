/*
** Builds with C++1 or heigher.
** RCS:
** $Log$
*/
#include <iostream>
#include <pthread.h>
#include <mutex>

// Shared data and mutex
int counter = 0;
std::mutex counter_mutex;

// Function to increment the counter, to be run by each thread
void* incrementCounter(void* arg) {
    int thread_id = *reinterpret_cast<int*>(arg);

    for (int i = 0; i < 1101; ++i) {
        std::lock_guard<std::mutex> lock(counter_mutex);  // Lock the mutex
        ++counter;
//        std::cout << "Thread " << thread_id << " incremented counter to " << counter << std::endl;
        // Mutex is automatically unlocked when the lock_guard goes out of scope
    }

    return nullptr;
}

int main() {
    const int NUM_THREADS = 50;
    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create multiple threads using pthread
    for (int i = 0; i < NUM_THREADS; ++i) {
        thread_ids[i] = i + 1; // Assign thread IDs (starting from 1)
        if (pthread_create(&threads[i], nullptr, incrementCounter, &thread_ids[i]) != 0) {
            std::cerr << "Error creating thread " << i + 1 << std::endl;
            return 1;
        }
    }

    // Join the threads (wait for them to finish)
    for (int i = 0; i < NUM_THREADS; ++i) {
        if (pthread_join(threads[i], nullptr) != 0) {
            std::cerr << "Error joining thread " << i + 1 << std::endl;
            return 1;
        }
    }

    std::cout << "Final counter value: " << counter << std::endl;

    return 0;
}
