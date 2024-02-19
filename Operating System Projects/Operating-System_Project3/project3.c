#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <sys/time.h>
// Global variables
double global_sqrt_sum = 0;
pthread_mutex_t mutex;
// Function prototypes
void* sqrtSumMethod1(void* arg);
void* sqrtSumMethod2(void* arg);
void* sqrtSumMethod3(void* arg);
void method1(long long a, long long b, int thread_count);
void method2(long long a, long long b, int thread_count);
void method3(long long a, long long b, int thread_count);
int main(int argc, char *argv[]) {
    // Check if the correct number of arguments is provided
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <start_range> <end_range> <thread_count> <method>\n", argv[0]);
        return 1;
    }

    // Parse command line arguments for range (a, b), thread count (c), and method (d)
    long long a = atoll(argv[1]);
    long long b = atoll(argv[2]);
    int c = atoi(argv[3]);
    int d = atoi(argv[4]);

    // Validate input values
    if (a > b || c <= 0 || (d < 1 || d > 3)) {
        fprintf(stderr, "Invalid input values.\n");
        return 1;
    }

    // Initialize mutex used for synchronization in methods 2 and 3
    pthread_mutex_init(&mutex, NULL);

    // Call the appropriate method based on the input
    switch (d) {
        case 1:
            method1(a, b, c);
            break;
        case 2:
            method2(a, b, c);
            break;
        case 3:
            method3(a, b, c);
            break;
        default:
            fprintf(stderr, "Invalid method number.\n");
            pthread_mutex_destroy(&mutex);
            return 1;
    }

    // Clean up mutex
    pthread_mutex_destroy(&mutex);

    // Print the final sum
    printf("Sum: %f\n", global_sqrt_sum);
    return 0;
}

// Thread function for Method 1: Each thread computes a part of the global sum without synchronization
void* sqrtSumMethod1(void* arg) {
    long long start = ((long long*)arg)[0]; // Start of the range for this thread
    long long end = ((long long*)arg)[1];   // End of the range for this thread
    for (long long i = start; i <= end; i++) {
        global_sqrt_sum += sqrt(i);  // Add the square root of i to the global sum
    }
    return NULL;
}

// Method 1: Spawns threads to calculate sqrt sum without synchronization
void method1(long long a, long long b, int thread_count) {
    pthread_t threads[thread_count]; // Array to hold thread identifiers
    long long range = (b - a + 1) / thread_count; // Calculate range for each thread
    long long start, end;

    // Create threads
    for (int i = 0; i < thread_count; i++) {
        start = a + i * range; // Calculate start of range for this thread
        end = (i == thread_count - 1) ? b : start + range - 1; // Calculate end of range

        // Allocate memory for thread arguments
        long long* args = malloc(2 * sizeof(long long));
        args[0] = start;
        args[1] = end;

        // Create thread
        if (pthread_create(&threads[i], NULL, sqrtSumMethod1, args) != 0) {
            perror("Failed to create thread"); // Handle thread creation failure
            exit(1);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL); // Join each thread with the main thread
    }
}

// Thread function for Method 2: Each thread computes a part of the global sum with synchronization
void* sqrtSumMethod2(void* arg) {
    long long start = ((long long*)arg)[0]; // Start of the range for this thread
    long long end = ((long long*)arg)[1];   // End of the range for this thread
    for (long long i = start; i <= end; i++) {
        pthread_mutex_lock(&mutex);          // Lock the mutex before updating the global sum
        global_sqrt_sum += sqrt(i);          // Add the square root of i to the global sum
        pthread_mutex_unlock(&mutex);        // Unlock the mutex after updating
    }
    return NULL;
}

// Method 2: Spawns threads to calculate sqrt sum with mutex synchronization
void method2(long long a, long long b, int thread_count) {
    pthread_t threads[thread_count]; // Array to hold thread identifiers
    long long range = (b - a + 1) / thread_count; // Calculate range for each thread
    long long start, end;

    // Create threads
    for (int i = 0; i < thread_count; i++) {
        start = a + i * range; // Calculate start of range for this thread
        end = (i == thread_count - 1) ? b : start + range - 1; // Calculate end of range

        // Allocate memory for thread arguments
        long long* args = malloc(2 * sizeof(long long));
        args[0] = start;
        args[1] = end;

        // Create thread
        if (pthread_create(&threads[i], NULL, sqrtSumMethod2, args) != 0) {
            perror("Failed to create thread"); // Handle thread creation failure
            exit(1);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL); // Join each thread with the main thread
    }
}

// Thread function for Method 3: Each thread computes its own local sum and then adds it to the global sum
void* sqrtSumMethod3(void* arg) {
    long long start = ((long long*)arg)[0]; // Start of the range for this thread
    long long end = ((long long*)arg)[1];   // End of the range for this thread
    double local_sqrt_sum = 0;

    // Compute the local sum of square roots
    for (long long i = start; i <= end; i++) {
        local_sqrt_sum += sqrt(i);
    }

    // Update the global sum with the local sum
    pthread_mutex_lock(&mutex);             // Lock the mutex before updating the global sum
    global_sqrt_sum += local_sqrt_sum;
    pthread_mutex_unlock(&mutex);           // Unlock the mutex after updating

    return NULL;
}

// Method 3: Spawns threads to calculate sqrt sum with local sums and mutex synchronization
void method3(long long a, long long b, int thread_count) {
    pthread_t threads[thread_count]; // Array to hold thread identifiers
    long long range = (b - a + 1) / thread_count; // Calculate range for each thread
    long long start, end;

    // Create threads
    for (int i = 0; i < thread_count; i++) {
        start = a + i * range; // Calculate start of range for this thread
        end = (i == thread_count - 1) ? b : start + range - 1; // Calculate end of range

        // Allocate memory for thread arguments
        long long* args = malloc(2 * sizeof(long long));
        args[0] = start;
        args[1] = end;

        // Create thread
        if (pthread_create(&threads[i], NULL, sqrtSumMethod3, args) != 0) {
            perror("Failed to create thread"); // Handle thread creation failure
            exit(1);
        }
    }

    // Wait for threads to finish
    for (int i = 0; i < thread_count; i++) {
        pthread_join(threads[i], NULL); // Join each thread with the main thread
    }
}
