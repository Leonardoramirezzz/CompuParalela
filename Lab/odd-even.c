#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 10000  // Size of the array to sort

void odd_even_sort_1(int a[], int n, int thread_count);
void odd_even_sort_2(int a[], int n, int thread_count);
void initialize_array(int a[], int n);
void print_array(int a[], int n);

int main() {
    int a[N], b[N];  // Two arrays to test both implementations
    int thread_counts[] = {1, 2, 3, 4};
    double start, end;
    
    for (int t = 0; t < 4; t++) {
        int thread_count = thread_counts[t];

        initialize_array(a, N);
        initialize_array(b, N);

        // Timing first implementation
        start = omp_get_wtime();
        odd_even_sort_1(a, N, thread_count);
        end = omp_get_wtime();
        printf("Two parallel for directives with %d threads: %.4f seconds\n", thread_count, end - start);

        // Timing second implementation
        start = omp_get_wtime();
        odd_even_sort_2(b, N, thread_count);
        end = omp_get_wtime();
        printf("Two for directives with %d threads: %.4f seconds\n", thread_count, end - start);
    }

    return 0;
}

// First implementation (with two parallel for directives)
void odd_even_sort_1(int a[], int n, int thread_count) {
    int phase, i, tmp;
    for (phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            #pragma omp parallel for num_threads(thread_count) default(none) shared(a, n) private(i, tmp)
            for (i = 1; i < n; i += 2) {
                if (a[i - 1] > a[i]) {
                    tmp = a[i - 1];
                    a[i - 1] = a[i];
                    a[i] = tmp;
                }
            }
        } else {
            #pragma omp parallel for num_threads(thread_count) default(none) shared(a, n) private(i, tmp)
            for (i = 1; i < n - 1; i += 2) {
                if (a[i] > a[i + 1]) {
                    tmp = a[i + 1];
                    a[i + 1] = a[i];
                    a[i] = tmp;
                }
            }
        }
    }
}

// Second implementation (with two non-parallel for directives)
void odd_even_sort_2(int a[], int n, int thread_count) {
    int phase, i, tmp;
    #pragma omp parallel num_threads(thread_count) default(none) shared(a, n) private(i, tmp, phase)
    for (phase = 0; phase < n; phase++) {
        if (phase % 2 == 0) {
            #pragma omp for
            for (i = 1; i < n; i += 2) {
                if (a[i - 1] > a[i]) {
                    tmp = a[i - 1];
                    a[i - 1] = a[i];
                    a[i] = tmp;
                }
            }
        } else {
            #pragma omp for
            for (i = 1; i < n - 1; i += 2) {
                if (a[i] > a[i + 1]) {
                    tmp = a[i + 1];
                    a[i + 1] = a[i];
                    a[i] = tmp;
                }
            }
        }
    }
}

void initialize_array(int a[], int n) {
    for (int i = 0; i < n; i++) {
        a[i] = rand() % 10000;  // Random integers
    }
}

void print_array(int a[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d ", a[i]);
    }
    printf("\n");
}
