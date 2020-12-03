#include <iostream>
#include <math.h>
#include <chrono>
#include <omp.h>

using namespace std;


double calcIntegral(long double A, long double B){
    
    long double ans;
    auto t1 = std::chrono::system_clock::now();
    ans = (0.25*(2*((B-A)/(B*A))+sin(2/B)-sin(2/A)));
    auto t2 = std::chrono::system_clock::now();
    cout<<"Seq result="<<ans<<"\n";
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
}


double calcIntegralParallel_a(long double A, long double B){

    long double ans;
    auto t1 = std::chrono::system_clock::now();

    #pragma omp parallel
    ans = (0.25*(2*((B-A)/(B*A))+sin(2/B)-sin(2/A)));
    auto t2 = std::chrono::system_clock::now();
    cout<<"Parallel result="<<ans<<"\n";
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();

}

int main(){
    
    long double A[7]={0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 10};
    long double B[7]={0.0001, 0.001, 0.01, 0.1, 1, 10, 100};
    double finalResult[7][6];
    
    for (int i=0; i<7; i++){
        finalResult[i][0] = A[i];
        finalResult[i][1] = B[i];
        finalResult[i][2] = calcIntegral(A[i], B[i]);
        finalResult[i][3] = calcIntegralParallel_a(A[i], B[i]);
    }

    cout<<"\nA\tB\tSeq_time(ns)";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t"<<finalResult[i][1]<<"\t"<<finalResult[i][2];
    }
    cout<<"\n";

    return 0;
}