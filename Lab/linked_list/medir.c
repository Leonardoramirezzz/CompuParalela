#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define MAX_ELEMENTS 1000000 // Adjust according to the size of the list you want
#define NUM_OPERATIONS 100000

struct list_node_s {
    int data;
    struct list_node_s* next;
    pthread_mutex_t mutex; // Mutex per node for finer locking
};

struct list_node_s* head = NULL;
pthread_mutex_t list_mutex;       // Single mutex for the entire list
pthread_rwlock_t rwlock;          // Read-write lock for the entire list
int num_threads;                  // Number of threads set dynamically

// Utility to get current time in seconds
double GetTime() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return t.tv_sec + t.tv_usec / 1000000.0;
}

// Member function with one mutex per node
int MemberNodeMutex(int value) {
    struct list_node_s* temp_p;

    pthread_mutex_lock(&head->mutex);
    temp_p = head;
    while (temp_p != NULL && temp_p->data < value) {
        if (temp_p->next != NULL) {
            pthread_mutex_lock(&(temp_p->next->mutex));
        }
        pthread_mutex_unlock(&(temp_p->mutex));
        temp_p = temp_p->next;
    }

    if (temp_p == NULL || temp_p->data > value) {
        if (temp_p != NULL) pthread_mutex_unlock(&(temp_p->mutex));
        return 0;
    } else {
        pthread_mutex_unlock(&(temp_p->mutex));
        return 1;
    }
}

// Member function with a single mutex for the entire list
int MemberListMutex(int value) {
    struct list_node_s* temp_p;

    pthread_mutex_lock(&list_mutex);
    temp_p = head;
    while (temp_p != NULL && temp_p->data < value)
        temp_p = temp_p->next;

    int result = (temp_p != NULL && temp_p->data == value);
    pthread_mutex_unlock(&list_mutex);
    return result;
}

// Member function with read-write locks
int MemberRWLock(int value) {
    struct list_node_s* temp_p;

    pthread_rwlock_rdlock(&rwlock);
    temp_p = head;
    while (temp_p != NULL && temp_p->data < value)
        temp_p = temp_p->next;

    int result = (temp_p != NULL && temp_p->data == value);
    pthread_rwlock_unlock(&rwlock);
    return result;
}

// Dummy thread function for the benchmark
void* ThreadWork(void* rank) {
    long my_rank = (long)rank;
    int count = NUM_OPERATIONS / num_threads;

    for (int i = 0; i < count; i++) {
        // Modify this to use the appropriate Member function
        // Example: MemberNodeMutex, MemberListMutex, MemberRWLock
        MemberRWLock(rand() % MAX_ELEMENTS); 
    }

    return NULL;
}

// Benchmark function
void RunBenchmark(char lock_type, int num_threads) {
    pthread_t* thread_handles;
    thread_handles = malloc(num_threads * sizeof(pthread_t));

    // Timing
    double start_time = GetTime();

    // Create threads
    for (long thread = 0; thread < num_threads; thread++) {
        pthread_create(&thread_handles[thread], NULL, ThreadWork, (void*)thread);
    }

    // Join threads
    for (long thread = 0; thread < num_threads; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    double end_time = GetTime();

    printf("Lock type: %c, Threads: %d, Time: %f seconds\n", lock_type, num_threads, end_time - start_time);

    free(thread_handles);
}

// Main function
int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <lock_type> <num_threads>\n", argv[0]);
        exit(1);
    }

    char lock_type = argv[1][0];  // 'm' for mutex, 'n' for node mutex, 'r' for rwlock
    num_threads = atoi(argv[2]);

    if (num_threads <= 0) {
        fprintf(stderr, "Error: Number of threads must be positive\n");
        exit(1);
    }

    // Initialize the head of the linked list
    head = malloc(sizeof(struct list_node_s));
    head->data = rand() % MAX_ELEMENTS;
    head->next = NULL;
    pthread_mutex_init(&head->mutex, NULL);

    // Initialize locks
    pthread_mutex_init(&list_mutex, NULL);
    pthread_rwlock_init(&rwlock, NULL);

    // Run benchmark based on the lock type
    switch (lock_type) {
        case 'm':  // One mutex for the entire list
            RunBenchmark('m', num_threads);
            break;
        case 'n':  // One mutex per node
            RunBenchmark('n', num_threads);
            break;
        case 'r':  // Read-write locks
            RunBenchmark('r', num_threads);
            break;
        default:
            fprintf(stderr, "Invalid lock type. Use 'm', 'n', or 'r'.\n");
            exit(1);
    }

    // Destroy locks
    pthread_mutex_destroy(&list_mutex);
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
