#include <iostream>
#include <vector>
#include <chrono> 

using namespace std;
using namespace std::chrono;

void classicMatrixMultiply(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int n) {
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            C[i][j] = 0;
            for (int k = 0; k < n; k++) {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }
}

void blockMatrixMultiply(const vector<vector<int>>& A, const vector<vector<int>>& B, vector<vector<int>>& C, int n, int blockSize) {
    for (int ii = 0; ii < n; ii += blockSize) {
        for (int jj = 0; jj < n; jj += blockSize) {
            for (int kk = 0; kk < n; kk += blockSize) {
                for (int i = ii; i < min(ii + blockSize, n); i++) {
                    for (int j = jj; j < min(jj + blockSize, n); j++) {
                        int sum = 0;
                        for (int k = kk; k < min(kk + blockSize, n); k++) {
                            sum += A[i][k] * B[k][j];
                        }
                        C[i][j] += sum;
                    }
                }
            }
        }
    }
}

int main() {
    int n;  
    int blockSize; 

    cout << "Introduce el tamaño de la matriz (n x n): ";
    cin >> n;
    cout << "Introduce el tamaño del bloque para la multiplicación por bloques: ";
    cin >> blockSize;

    vector<vector<int>> A(n, vector<int>(n));
    vector<vector<int>> B(n, vector<int>(n));
    vector<vector<int>> C(n, vector<int>(n, 0));
    vector<vector<int>> D(n, vector<int>(n, 0));
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % 10;  
            B[i][j] = rand() % 10;  
        }
    }

    auto startClassic = high_resolution_clock::now();
    classicMatrixMultiply(A, B, C, n);
    auto stopClassic = high_resolution_clock::now();
    auto durationClassic = duration_cast<milliseconds>(stopClassic - startClassic);
    cout << "Tiempo de ejecución (multiplicación clásica): " << durationClassic.count() << " milisegundos." << endl;

    auto startBlock = high_resolution_clock::now();
    blockMatrixMultiply(A, B, D, n, blockSize);
    auto stopBlock = high_resolution_clock::now();
    auto durationBlock = duration_cast<milliseconds>(stopBlock - startBlock);
    cout << "Tiempo de ejecución (multiplicación por bloques): " << durationBlock.count() << " milisegundos." << endl;
    cout << "Comparación de tiempos: " << endl;
    cout << "Clásica: " << durationClassic.count() << " ms, Por bloques: " << durationBlock.count() << " ms." << endl;

    return 0;
}
