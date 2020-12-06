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
    cout<<"A = "<<A<<"     \tB = "<<B<<"     \tResult="<<ans<<"\n";
    return std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();
}


double calcIntegralParallel_a(long double A, long double B, int threads){

    long double ans=0.0, x=0.0, y=0.0;
    auto t1 = std::chrono::system_clock::now();
    #pragma omp parallel num_threads(threads)
        x = 2*((B-A)/(B*A));
        y = sin(2/B)-sin(2/A);
        #pragma omp atomic                  //atomic
            ans+=0.25*(x+y);
    auto t2 = std::chrono::system_clock::now();
    //cout<<"Parallel result="<<ans<<"\n";
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

}

double calcIntegralParallel_b(long double A, long double B, int threads){

    long double ans, x, y;
    auto t1 = std::chrono::system_clock::now();
    #pragma omp parallel num_threads(threads)
        x = (0.25*(2*((B-A)/(B*A))));
        y = (0.25*(sin(2/B)-sin(2/A)));
        #pragma omp critical                //critical
            ans=(x+y);
    auto t2 = std::chrono::system_clock::now();
    //cout<<"Parallel result="<<ans<<"\n";
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

}

double calcIntegralParallel_c(long double A, long double B, int threads){

    long double ans, x, y;
    auto t1 = std::chrono::system_clock::now();
    omp_lock_t lock1;
    omp_init_lock(&lock1);
    #pragma omp parallel num_threads(threads)
        x = 2*((B-A)/(B*A));
        y = sin(2/B)-sin(2/A);
        omp_set_lock(&lock1);               
        ans=0.25*(x+y);                     //lock
        omp_unset_lock(&lock1);
    omp_destroy_lock(&lock1);
    auto t2 = std::chrono::system_clock::now();
    //cout<<"Parallel result="<<ans<<"\n";
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

}


double calcIntegralParallel_d(long double A, long double B, int threads){

    long double ans, x, y;
    auto t1 = std::chrono::system_clock::now();
    #pragma omp parallel num_threads(threads)
        x = 2*((B-A)/(B*A));
        y = sin(2/B)-sin(2/A);
        #pragma omp reduction(+:ans)        //reduction
            ans=0.25*(x+y);
    auto t2 = std::chrono::system_clock::now();
    //cout<<"Parallel result="<<ans<<"\n";
    return std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();

}


int main(){
    
    long double A[7]={0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 10};
    long double B[7]={0.0001, 0.001, 0.01, 0.1, 1, 10, 100};
    double finalResult[7][20];
    
    for (int i=0; i<7; i++){
        finalResult[i][0] = A[i];
        finalResult[i][1] = B[i];
        finalResult[i][2] = calcIntegral(A[i], B[i]);

        finalResult[i][3] = calcIntegralParallel_a(A[i], B[i], 3);
        finalResult[i][4] = calcIntegralParallel_a(A[i], B[i], 4);
        finalResult[i][5] = calcIntegralParallel_a(A[i], B[i], 5);

        finalResult[i][6] = calcIntegralParallel_b(A[i], B[i], 3);
        finalResult[i][7] = calcIntegralParallel_b(A[i], B[i], 4);
        finalResult[i][8] = calcIntegralParallel_b(A[i], B[i], 5);

        finalResult[i][9] = calcIntegralParallel_c(A[i], B[i], 3);
        finalResult[i][10] = calcIntegralParallel_c(A[i], B[i], 4);
        finalResult[i][11] = calcIntegralParallel_c(A[i], B[i], 5);

        finalResult[i][12] = calcIntegralParallel_d(A[i], B[i], 3);
        finalResult[i][13] = calcIntegralParallel_d(A[i], B[i], 4);
        finalResult[i][14] = calcIntegralParallel_d(A[i], B[i], 5);

    }

    cout<<"\nA\tB\tT_S(ns)\tT_P_A_3(ns)\tT_P_A_4(ns)\tT_P_A_5(ns)";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t"<<finalResult[i][1]<<"\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][3]<<"\t"<<finalResult[i][4]
        <<"\t"<<finalResult[i][5];
    }
    cout<<"\n";

    cout<<"\nA\tB\tT_S(ns)\tT_P_B_3(ns)\tT_P_B_4(ns)\tT_P_B_5(ns)";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t"<<finalResult[i][1]<<"\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][6]<<"\t"<<finalResult[i][7]
        <<"\t"<<finalResult[i][8];
    }
    cout<<"\n";

    cout<<"\nA\tB\tT_S(ns)\tT_P_C_3(ns)\tT_P_C_4(ns)\tT_P_C_5(ns)";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t"<<finalResult[i][1]<<"\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][9]<<"\t"<<finalResult[i][10]
        <<"\t"<<finalResult[i][11];
    }
    cout<<"\n";

    cout<<"\nA\tB\tT_S(ns)\tT_P_D_3(ns)\tT_P_D_4(ns)\tT_P_D_5(ns)";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t"<<finalResult[i][1]<<"\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][12]<<"\t"<<finalResult[i][13]
        <<"\t"<<finalResult[i][14];
    }
    cout<<"\n";

    return 0;
}