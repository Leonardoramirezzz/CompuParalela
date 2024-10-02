#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Define the structure for the nodes in the linked list, each with its own mutex
struct list_node_s {
    int data;
    struct list_node_s* next;
    pthread_mutex_t mutex;  // Mutex to protect each individual node
};

// Global head pointer for the linked list
struct list_node_s* head_p = NULL;
pthread_mutex_t head_p_mutex = PTHREAD_MUTEX_INITIALIZER;  // Protects access to the head pointer

// Member function (to check if a value exists in the list)
int Member(int value) {
    struct list_node_s* temp_p;

    // Lock head mutex before accessing the list
    pthread_mutex_lock(&head_p_mutex);
    temp_p = head_p;

    // If there's a node, lock its mutex and release head mutex
    if (temp_p != NULL) {
        pthread_mutex_lock(&(temp_p->mutex));
    }
    pthread_mutex_unlock(&head_p_mutex);

    // Traverse the list to find the value
    while (temp_p != NULL && temp_p->data < value) {
        struct list_node_s* next_p = temp_p->next;
        if (next_p != NULL) {
            pthread_mutex_lock(&(next_p->mutex));
        }
        pthread_mutex_unlock(&(temp_p->mutex));
        temp_p = next_p;
    }

    if (temp_p == NULL || temp_p->data > value) {
        if (temp_p != NULL) {
            pthread_mutex_unlock(&(temp_p->mutex));
        }
        return 0;
    } else {
        pthread_mutex_unlock(&(temp_p->mutex));
        return 1;
    }
}

// Insert function (to insert a new value into the list)
int Insert(int value) {
    pthread_mutex_lock(&head_p_mutex);
    struct list_node_s* curr_p = head_p;
    struct list_node_s* pred_p = NULL;
    struct list_node_s* temp_p;

    if (curr_p != NULL) {
        pthread_mutex_lock(&(curr_p->mutex));  // Lock current node's mutex
    }
    pthread_mutex_unlock(&head_p_mutex);  // Unlock head mutex

    // Traverse the list to find the correct position
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
        if (curr_p != NULL) {
            pthread_mutex_lock(&(curr_p->mutex));  // Lock next node's mutex
        }
        pthread_mutex_unlock(&(pred_p->mutex));  // Unlock the previous node's mutex
    }

    // Check if value is already in the list
    if (curr_p == NULL || curr_p->data > value) {
        temp_p = malloc(sizeof(struct list_node_s));
        if (temp_p == NULL) {  // Memory allocation failed
            if (curr_p != NULL) {
                pthread_mutex_unlock(&(curr_p->mutex));
            }
            return 0;
        }
        temp_p->data = value;
        temp_p->next = curr_p;
        pthread_mutex_init(&(temp_p->mutex), NULL);  // Initialize the mutex for the new node

        if (pred_p == NULL) {  // Insert as the first node
            pthread_mutex_lock(&head_p_mutex);
            head_p = temp_p;
            pthread_mutex_unlock(&head_p_mutex);
        } else {
            pred_p->next = temp_p;
            pthread_mutex_unlock(&(pred_p->mutex));
        }

        if (curr_p != NULL) {
            pthread_mutex_unlock(&(curr_p->mutex));
        }
        return 1;
    } else {  // Value is already in the list
        if (curr_p != NULL) {
            pthread_mutex_unlock(&(curr_p->mutex));
        }
        return 0;
    }
}

// Delete function (to delete a value from the list)
int Delete(int value) {
    pthread_mutex_lock(&head_p_mutex);
    struct list_node_s* curr_p = head_p;
    struct list_node_s* pred_p = NULL;

    if (curr_p != NULL) {
        pthread_mutex_lock(&(curr_p->mutex));  // Lock current node's mutex
    }
    pthread_mutex_unlock(&head_p_mutex);  // Unlock head mutex

    // Traverse the list to find the node to delete
    while (curr_p != NULL && curr_p->data < value) {
        pred_p = curr_p;
        curr_p = curr_p->next;
        if (curr_p != NULL) {
            pthread_mutex_lock(&(curr_p->mutex));  // Lock next node's mutex
        }
        pthread_mutex_unlock(&(pred_p->mutex));  // Unlock the previous node's mutex
    }

    if (curr_p != NULL && curr_p->data == value) {  // Value found
        if (pred_p == NULL) {  // Deleting the first node
            pthread_mutex_lock(&head_p_mutex);
            head_p = curr_p->next;
            pthread_mutex_unlock(&head_p_mutex);
        } else {
            pred_p->next = curr_p->next;
            pthread_mutex_unlock(&(pred_p->mutex));
        }
        pthread_mutex_unlock(&(curr_p->mutex));
        pthread_mutex_destroy(&(curr_p->mutex));  // Destroy the mutex of the deleted node
        free(curr_p);  // Free the memory
        return 1;
    } else {  // Value not found
        if (curr_p != NULL) {
            pthread_mutex_unlock(&(curr_p->mutex));
        }
        return 0;
    }
}

// Function to print the list (for debugging purposes)
void PrintList() {
    pthread_mutex_lock(&head_p_mutex);
    struct list_node_s* curr_p = head_p;
    if (curr_p != NULL) {
        pthread_mutex_lock(&(curr_p->mutex));  // Lock the mutex of the first node
    }
    pthread_mutex_unlock(&head_p_mutex);  // Unlock head mutex

    printf("List: ");
    while (curr_p != NULL) {
        printf("%d -> ", curr_p->data);
        struct list_node_s* next_p = curr_p->next;
        if (next_p != NULL) {
            pthread_mutex_lock(&(next_p->mutex));  // Lock the next node
        }
        pthread_mutex_unlock(&(curr_p->mutex));  // Unlock the current node
        curr_p = next_p;
    }
    printf("NULL\n");
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
