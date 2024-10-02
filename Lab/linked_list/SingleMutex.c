#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the structure for the nodes in the linked list
struct list_node_s {
    int data;
    struct list_node_s* next;
};

// Global variables: head pointer and a single mutex for the list
struct list_node_s* head_p = NULL;
pthread_mutex_t list_mutex = PTHREAD_MUTEX_INITIALIZER;

// Member function (to check if a value exists in the list)
int Member(int value) {
    pthread_mutex_lock(&list_mutex);
    struct list_node_s* curr_p = head_p;
    
    while (curr_p != NULL && curr_p->data < value) {
        curr_p = curr_p->next;
    }

    if (curr_p == NULL || curr_p->data > value) {
        pthread_mutex_unlock(&list_mutex);
        return 0;
    } else {
        pthread_mutex_unlock(&list_mutex);
        return 1;
    }
}

// Insert function (to insert a new value into the list)
int Insert(int value) {
    pthread_mutex_lock(&list_mutex);

    struct list_node_s* curr_p = head_p;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    // Traverse the list to find the correct position
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    // Check if value is already in the list
    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        if (temp_p == NULL) { // Memory allocation failed
            pthread_mutex_unlock(&list_mutex);
            return 0;
        }
        temp_p->data = value;
        temp_p->next = curr_p;

        if (pred_p == NULL) { // Insert as the first node
            head_p = temp_p;
        } else {
            pred_p->next = temp_p;
        }
        pthread_mutex_unlock(&list_mutex);
        return 1;
    } else { // Value is already in the list
        pthread_mutex_unlock(&list_mutex);
        return 0;
    }
}

// Delete function (to delete a value from the list)
int Delete(int value) {
    pthread_mutex_lock(&list_mutex);

    struct list_node_s* curr_p = head_p;
    struct list_node_s* pred_p = NULL;

    // Traverse the list to find the node to delete
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
    }

    if (curr_p != NULL && curr_p->data == value) { // Value found
        if (pred_p == NULL) { // Deleting the first node
            head_p = curr_p->next;
        } else {
            pred_p->next = curr_p->next;
        }
        free(curr_p); // Free the memory
        pthread_mutex_unlock(&list_mutex);
        return 1;
    } else { // Value not found
        pthread_mutex_unlock(&list_mutex);
        return 0;
    }
}

// Function to print the list (for debugging purposes)
void PrintList() {
    pthread_mutex_lock(&list_mutex);
    struct list_node_s* curr_p = head_p;
    printf("List: ");
    while (curr_p != NULL) {
        printf("%d -> ", curr_p->data);
        curr_p = curr_p->next;
    }
    printf("NULL\n");
    pthread_mutex_unlock(&list_mutex);
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

    return 0;
}
