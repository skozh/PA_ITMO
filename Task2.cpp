/* Matrix Multiplication */
#include <iostream>
#include <omp.h>
#include <chrono>
using namespace std;


double multiplyMatrices(int A[][100], int B[][100], int C[][100], int size){
    
    auto t1 = std::chrono::system_clock::now();
    for (int i=0; i<size; i++){
        for (int j=0; j<size; j++){
            C[i][j]=0;
            for (int k=0; k<size; k++){
                C[i][j]+= A[i][k]*B[k][j];
            }
        }
    }
    auto t2 = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count(); 
}


double multiplyMatricesParallel_1(int A[][100], int B[][100], int D[][100], int size){

    int i, j, k;
    omp_set_dynamic(0);
    omp_set_num_threads(size);
    auto t1 = std::chrono::system_clock::now();
    #pragma omp parallel for private(i)
        for (i=0; i<size; i++){
            for (j=0; j<size; j++){
                D[i][j]=0;
                for (k=0; k<size; k++){
                    D[i][j]+= A[i][k]*B[k][j];
                }
            }
        }
    auto t2 = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
}


double multiplyMatricesParallel_2(int A[][100], int B[][100], int D[][100], int size){

    int i, j, k;
    omp_set_dynamic(0);
    omp_set_num_threads(size);
    auto t1 = std::chrono::system_clock::now();
    #pragma omp parallel for private(j)
        for (i=0; i<size; i++){
            for (j=0; j<size; j++){
                D[i][j]=0;
                for (k=0; k<size; k++){
                    D[i][j]+= A[i][k]*B[k][j];
                }
            }
        }
    auto t2 = std::chrono::system_clock::now();
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count(); 
}


void displayMatrix(int A[100][100], int size){
    
    for (int i=0; i< size; i++){
        for (int j=0; j<size; j++){
            cout<<A[i][j];
            cout<<"\t";
        }
        cout<<"\n";
    }
}


int main()
{

    int A[100][100], B[100][100], C[100][100], D[100][100], size;
    std::clock_t start, end, t1, t2;
    double duration_1, duration_2, duration_3;
    srand(time(0));
    
    cout<<"Enter size of the square matrices: ";
    cin>>size;

    for (int i=0; i< size; i++){
        for (int j=0; j<size; j++){
            A[i][j]=1+rand()%9;
            B[i][j]=1+rand()%9;
        }
    }

    cout<<"Matrix A\n";
    displayMatrix(A, size);

    cout<<"Matrix B\n";
    displayMatrix(B, size);


    //Sequential method
    duration_1 = multiplyMatrices(A, B, C, size);

    //Parallel methods
    duration_2 = multiplyMatricesParallel_1(A, B, D, size);
    duration_3 = multiplyMatricesParallel_2(A, B, D, size);
    
    cout<<"Matrix C\n";
    displayMatrix(C, size);

    cout<<"Matrix D\n";
    displayMatrix(D, size);

    cout<<"Sequential Time (ns): "<<to_string(duration_1)<<"\n";
    cout<<"Parallel Time #1 (ns): "<<to_string(duration_2)<<"\n";
    cout<<"Parallel Time #2 (ns): "<<to_string(duration_3)<<"\n";
    
    return 0;
}