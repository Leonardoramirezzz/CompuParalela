#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the structure for the nodes in the linked list
struct list_node_s {
    int data;
    struct list_node_s* next;
};

// Global head pointer for the linked list
struct list_node_s* head_p = NULL;
pthread_rwlock_t rwlock;  // Read-Write lock for the entire list

// Member function (to check if a value exists in the list)
int Member(int value) {
    struct list_node_s* temp_p;

    // Lock the read-write lock for reading
    pthread_rwlock_rdlock(&rwlock);

    temp_p = head_p;
    while (temp_p != NULL && temp_p->data < value) {
        temp_p = temp_p->next;
    }

    if (temp_p == NULL || temp_p->data > value) {
        // Unlock the read-write lock after reading
        pthread_rwlock_unlock(&rwlock);
        return 0;
    } else {
        pthread_rwlock_unlock(&rwlock);
        return 1;
    }
}

// Insert function (to insert a new value into the list)
int Insert(int value) {
    pthread_rwlock_wrlock(&rwlock);  // Lock the read-write lock for writing

    struct list_node_s* curr_p = head_p;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    // Traverse the list to find the correct position
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // Check if the value is already in the list
    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        if (temp_p == NULL) {  // Memory allocation failed
            pthread_rwlock_unlock(&rwlock);  // Unlock write lock before returning
            return 0;
        }
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL) {  // Insert as the first node
            head_p = temp_p;
        } else {
            pred_p->next = temp_p;
        }

        pthread_rwlock_unlock(&rwlock);  // Unlock the write lock
        return 1;
    } else {  // Value is already in the list
        pthread_rwlock_unlock(&rwlock);  // Unlock write lock
        return 0;
    }
}

// Delete function (to delete a value from the list)
int Delete(int value) {
    pthread_rwlock_wrlock(&rwlock);  // Lock the read-write lock for writing

    struct list_node_s* curr_p = head_p;
    struct list_node_s* pred_p = NULL;

    // Traverse the list to find the node to delete
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // If the value is found, delete the node
    if (curr_p != NULL && curr_p->data == value) {
        if (pred_p == NULL) {  // Deleting the first node
            head_p = curr_p->next;
        } else {
            pred_p->next = curr_p->next;
        }
        free(curr_p);  // Free the memory of the deleted node
        pthread_rwlock_unlock(&rwlock);  // Unlock the write lock
        return 1;
    } else {  // Value not found
        pthread_rwlock_unlock(&rwlock);  // Unlock write lock
        return 0;
    }
}

// Function to print the list (for debugging purposes)
void PrintList() {
    pthread_rwlock_rdlock(&rwlock);  // Lock the read-write lock for reading

    struct list_node_s* curr_p = head_p;

    printf("List: ");
    while (curr_p != NULL) {
        printf("%d -> ", curr_p->data);
        curr_p = curr_p->next;
    }
    printf("NULL\n");

    pthread_rwlock_unlock(&rwlock);  // Unlock the read-write lock after reading
}

// Thread function to simulate operations on the list
void* ThreadWork(void* rank) {
    long my_rank = (long) rank;
    
    // Perform some operations (this can be customized as needed)
    if (my_rank % 2 == 0) {
        Insert(my_rank);
    } else {
        Member(my_rank);
    }

    return NULL;
}

// Main function to test the multi-threaded linked list
int main() {
    long thread;
    pthread_t thread_handles[4];
    int num_threads = 4;

    // Initialize the read-write lock
    pthread_rwlock_init(&rwlock, NULL);

    // Create threads to perform operations on the list
    for (thread = 0; thread < num_threads; thread++) {
        pthread_create(&thread_handles[thread], NULL, ThreadWork, (void*) thread);
    }

    // Join the threads
    for (thread = 0; thread < num_threads; thread++) {
        pthread_join(thread_handles[thread], NULL);
    }

    // Print the final list
    PrintList();

    // Destroy the read-write lock
    pthread_rwlock_destroy(&rwlock);

    return 0;
}
