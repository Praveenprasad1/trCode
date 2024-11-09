#include <stdio.h>
#include <pthread.h>

#if defined(__APPLE__) && defined(__MACH__)
    // MacOS-specific imports
    #include <os/lock.h>
#else
    // Other platforms (Linux, etc.)
    #include <pthread.h>
#endif

// Shared counter
int shared_counter = 0;

// Lock declaration
#if defined(__APPLE__) && defined(__MACH__)
    os_unfair_lock unfair_lock = OS_UNFAIR_LOCK_INIT;
#else
    pthread_spinlock_t unfair_lock;
#endif

// Increment function
void* increment(void* arg) {
    for (int i = 0; i < 1000; i++) {
        // Platform-specific lock
        #if defined(__APPLE__) && defined(__MACH__)
            os_unfair_lock_lock(&unfair_lock);
            shared_counter++;
            os_unfair_lock_unlock(&unfair_lock);
        #else
            pthread_spin_lock(&unfair_lock);
            shared_counter++;
            pthread_spin_unlock(&unfair_lock);
        #endif
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Platform-specific lock initialization
    #if defined(__APPLE__) && defined(__MACH__)
        // No initialization required for os_unfair_lock
    #else
        pthread_spin_init(&unfair_lock, 0);
    #endif

    // Create threads
    pthread_create(&t1, NULL, increment, NULL);
    pthread_create(&t2, NULL, increment, NULL);

    // Join threads
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Output final result
    printf("Final counter value: %d\n", shared_counter);

    // Platform-specific lock destruction
    #if defined(__APPLE__) && defined(__MACH__)
        // No destruction required for os_unfair_lock
    #else
        pthread_spin_destroy(&unfair_lock);
    #endif

    return 0;
}
