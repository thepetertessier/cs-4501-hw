#include <iostream>
#include <vector>
#include <cmath>
#include <sstream>
#include <iomanip>

#define MAX_N 100000
#define MAX_Q 50000

using namespace std;
using Matrix = array<array<double, 3>, 3>;

Matrix multiply_matrices(Matrix &A, Matrix &B) {
    Matrix result = {};
    for (int i = 0; i < 3; i++)
        for (int k = 0; k < 3; k++)
            for (int j = 0; j < 3; j++)
                result[i][j] += A[i][k] * B[k][j];
    return result;
}

Matrix get_identity() {
    return {{
        {1,0,0},
        {0,1,0},
        {0,0,1}
    }};
}

// Function to print a matrix
void print_matrix(const Matrix& M) {
    for (const auto& row : M) {
        for (double val : row) {
            std::cout << val << " ";
        }
        std::cout << "\n";
    }
}

// Main function
int main() {
    // Define two 3x3 matrices
    Matrix A = {{
        {1.0, 2.0, 3.0},
        {4.0, 5.0, 6.0},
        {7.0, 8.0, 9.0}
    }};

    A = get_identity();
    
    Matrix B = {{
        {9.0, 8.0, 7.0},
        {6.0, 5.0, 4.0},
        {3.0, 2.0, 1.0}
    }};

    std::cout << "Matrix A:\n";
    print_matrix(A);

    std::cout << "\nMatrix B:\n";
    print_matrix(B);

    // Multiply matrices
    auto C = multiply_matrices(A, B);

    std::cout << "\nResulting Matrix C (A * B):\n";
    print_matrix(C);

    return 0;
}
