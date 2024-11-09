/*
** $Log$
*/
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

// Shared data and rwlock
int shared_data = 0;
pthread_rwlock_t rwlock;

// Reader function (multiple threads can read concurrently)
void* reader(void* arg) {
    int id = *(int*)arg;

    // Acquire the lock in read mode
    pthread_rwlock_rdlock(&rwlock);
    printf("Reader %d: read shared_data = %d\n", id, shared_data);
    // Simulate reading time
    usleep(100000);  // 100 ms
    // Release the read lock
    pthread_rwlock_unlock(&rwlock);

    return NULL;
}

// Writer function (only one thread can write at a time)
void* writer(void* arg) {
    int id = *(int*)arg;

    // Acquire the lock in write mode
    pthread_rwlock_wrlock(&rwlock);
    shared_data++;  // Modify the shared data
    printf("Writer %d: wrote shared_data = %d\n", id, shared_data);
    // Simulate writing time
    usleep(200000);  // 200 ms
    // Release the write lock
    pthread_rwlock_unlock(&rwlock);

    return NULL;
}

int main() {
    pthread_t readers[5], writers[2];
    int thread_ids[5];

    // Initialize the read-write lock
    pthread_rwlock_init(&rwlock, NULL);

    // Create reader threads
    for (int i = 0; i < 5; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&readers[i], NULL, reader, &thread_ids[i]);
    }

    // Create writer threads
    for (int i = 0; i < 2; i++) {
        thread_ids[i] = i + 1;
        pthread_create(&writers[i], NULL, writer, &thread_ids[i]);
//        usleep(150000);  // Delay between writer threads
    }

    // Wait for all threads to finish
    for (int i = 0; i < 5; i++) {
        pthread_join(readers[i], NULL);
    }
    for (int i = 0; i < 2; i++) {
        pthread_join(writers[i], NULL);
    }

    // Destroy the read-write lock
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
