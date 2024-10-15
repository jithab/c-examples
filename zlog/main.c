#include <zlog.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>  // For usleep()
#include <zlog.h>

#define NUM_THREADS 8
#define LOG_INTERVAL 50000  // 500 milliseconds in microseconds

// Function that each thread will run
void *log_function(void *arg) {
    int thread_id = *(int *)arg;

    // Log messages periodically
    while (1) {
        dzlog_info("Thread %d: Logging periodically", thread_id);
        usleep(LOG_INTERVAL);  // Sleep for 500 milliseconds
    }
    
    return NULL;
}

int main() {
    // Initialize zlog
    int rc = dzlog_init("zlog.conf", "my_category");
    if (rc) {
        fprintf(stderr, "zlog initialization failed: %d\n", rc);
        return -1;
    }

    pthread_t threads[NUM_THREADS];
    int thread_ids[NUM_THREADS];

    // Create and start threads
    for (int i = 0; i < NUM_THREADS; i++) {
        thread_ids[i] = i;  // Assign thread IDs
        rc = pthread_create(&threads[i], NULL, log_function, &thread_ids[i]);
        if (rc) {
            fprintf(stderr, "Error creating thread %d: %d\n", i, rc);
            zlog_fini();
            return -1;
        }
    }

    // Let the threads run for a while (e.g., 5 seconds)
    sleep(15);

    // Optionally, you can add a mechanism to stop the threads cleanly

    // Join the threads (in this example, we won't actually stop them)
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_cancel(threads[i]);  // Cancel the threads (not clean, but for demo)
    }

    // Clean up
    zlog_fini();
    return 0;
}
