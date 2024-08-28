#include <iostream>
#include <chrono>
#include <vector>
#include <cstdlib>
#include <ctime>

const int max = 100;

int main() {
    double A[max][max], x[max], y[max];

    srand(time(0));
    for (int i = 0; i < max; i++) {
        x[i] = rand() % 100;
        y[i] = 0;
        for (int j = 0; j < max; j++) {
            A[i][j] = rand() % 100;
        }
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < max; i++)
        for (int j = 0; j < max; j++)
            y[i] += A[i][j] * x[j];

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;
    std::cout << "Tiempo del primer par de bucles: " << duration.count() << " segundos" << std::endl;

    for (int i = 0; i < max; i++) {
        y[i] = 0;
    }

    start = std::chrono::high_resolution_clock::now();

    for (int j = 0; j < max; j++)
        for (int i = 0; i < max; i++)
            y[i] += A[i][j] * x[j];

    end = std::chrono::high_resolution_clock::now();
    duration = end - start;
    std::cout << "Tiempo del segundo par de bucles: " << duration.count() << " segundos" << std::endl;

    return 0;
}
