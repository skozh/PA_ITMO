#include <iostream>
#include <math.h>
#include <omp.h>

using namespace std;

double calcIntegral(long double A, long double B){
    
    long double ans=0.0;
    double t1=0, t2=0;
    t1 = omp_get_wtime();
    ans = (0.25*(2*((B-A)/(B*A))+sin(2/B)-sin(2/A)));
    t2 = omp_get_wtime();
    cout<<"A = "<<A<<"     \tB = "<<B<<"     \tResult="<<ans<<"\n";
    return (t2-t1);
}


double calcIntegralParallel_a(long double A, long double B, int threads){

    long double ans=0.0, x=0.0, y=0.0;
    double t1=0, t2=0;
    t1 = omp_get_wtime();
    x = 2*((B-A)/(B*A));
    y = sin(2/B)-sin(2/A);
    #pragma omp atomic                  //atomic
        ans+=0.25*(x+y);
    t2 = omp_get_wtime();
    //#pragma omp critical
    //    cout<<"\nParallel result (Atomic)\t"<<A<<"\t"<<B<<"\t"<<threads<<"\t"<<ans<<"\n";
    return (t2-t1);

}

double calcIntegralParallel_b(long double A, long double B, int threads){

    long double ans=0.0, x=0.0, y=0.0;
    double t1=0, t2=0;
    t1 = omp_get_wtime();
    x = (0.25*(2*((B-A)/(B*A))));
    y = (0.25*(sin(2/B)-sin(2/A)));
    #pragma omp critical                //critical
        ans=(x+y);
    //#pragma omp critical    
    //    cout<<"\nParallel result (Critical)\t"<<A<<"\t"<<B<<"\t"<<threads<<"\t"<<ans<<"\n";
    t2 = omp_get_wtime();
    return (t2-t1);

}

double calcIntegralParallel_c(long double A, long double B, int threads){

    long double ans=0, x=0, y=0;
    double t1=0, t2=0;
    t1 = omp_get_wtime();
    omp_lock_t lock1;
    omp_init_lock(&lock1);
    x = 2*((B-A)/(B*A));
    y = sin(2/B)-sin(2/A);
    omp_set_lock(&lock1);               
    ans=0.25*(x+y);                     //lock
    omp_unset_lock(&lock1);
    omp_destroy_lock(&lock1);
    t2 = omp_get_wtime();
    //#pragma omp critical
    //    cout<<"\nParallel result (Locks)  \t"<<A<<"\t"<<B<<"\t"<<threads<<"\t"<<ans<<"\n";
    return (t2-t1);

}


double calcIntegralParallel_d(long double A, long double B, int threads){

    long double ans=0, x=0, y=0;
    double t1=0, t2=0;
    t1 = omp_get_wtime();
    x = 2*((B-A)/(B*A));
    y = sin(2/B)-sin(2/A);
    #pragma omp reduction(+:ans)        //reduction
        ans=0.25*(x+y);
    t2 = omp_get_wtime();
    //#pragma omp critical
    //    cout<<"\nParallel result (Reduction)\t"<<A<<"\t"<<B<<"\t"<<threads<<"\t"<<ans<<"\n";
    return (t2-t1);

}


int main(){
    
    long double A[7]={0.00001, 0.0001, 0.001, 0.01, 0.1, 1, 10};
    long double B[7]={0.0001, 0.001, 0.01, 0.1, 1, 10, 100};
    double finalResult[7][20];
    int i;

    cout<<"\nResult:\n";
    for (i=0; i<7; i++){
        finalResult[i][0] = A[i];
        finalResult[i][1] = B[i];
        finalResult[i][2] = calcIntegral(A[i], B[i]);
    }


    int threads = 3;

    #pragma omp parallel private(i) num_threads(threads)   
        for (i=0; i<7; i++){
            finalResult[i][3] = calcIntegralParallel_a(A[i], B[i], threads);
            finalResult[i][6] = calcIntegralParallel_b(A[i], B[i], threads);
            finalResult[i][9] = calcIntegralParallel_c(A[i], B[i], threads);
            finalResult[i][12] = calcIntegralParallel_d(A[i], B[i], threads);
        }

    threads = 4;

    #pragma omp parallel private(i) num_threads(threads)   
        for (i=0; i<7; i++){
            finalResult[i][4] = calcIntegralParallel_a(A[i], B[i], threads);
            finalResult[i][7] = calcIntegralParallel_b(A[i], B[i], threads);
            finalResult[i][10] = calcIntegralParallel_c(A[i], B[i], threads);
            finalResult[i][13] = calcIntegralParallel_d(A[i], B[i], threads);
        }

    threads = 5;

    #pragma omp parallel private(i) num_threads(threads)   
        for (i=0; i<7; i++){
            finalResult[i][5] = calcIntegralParallel_a(A[i], B[i], threads);
            finalResult[i][8] = calcIntegralParallel_b(A[i], B[i], threads);
            finalResult[i][11] = calcIntegralParallel_c(A[i], B[i], threads);
            finalResult[i][14] = calcIntegralParallel_d(A[i], B[i], threads);
        }
    
    cout<<"\nAtomic:\n";

    cout<<"\nA\tB\tT_S\t\tT_P_A_3\t\t\tT_P_A_4\t\t\tT_P_A_5";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t"<<finalResult[i][1]<<"\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][3]<<"\t\t"<<finalResult[i][4]
        <<"\t\t"<<finalResult[i][5];
    }
    cout<<"\n\n";

    cout<<"\nCritical:\n";

    cout<<"\nA\tB\tT_S\t\tT_P_B_3\t\t\tT_P_B_4\t\t\tT_P_B_5";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t"<<finalResult[i][1]<<"\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][6]<<"\t\t"<<finalResult[i][7]
        <<"\t\t"<<finalResult[i][8];
    }
    cout<<"\n\n";

    cout<<"\nLocks:\n";

    cout<<"\nA\t\tB\t\tT_S\t\tT_P_C_3\t\t\tT_P_C_4\t\t\tT_P_C_5";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t\t"<<finalResult[i][1]<<"\t\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][9]<<"\t\t"<<finalResult[i][10]
        <<"\t\t"<<finalResult[i][11];
    }
    cout<<"\n\n";

    cout<<"\nReduction:\n";

    cout<<"\nA\t\tB\t\tT_S\t\tT_P_D_3\t\t\tT_P_D_4\t\t\tT_P_D_5";
    for (int i=0; i<7; i++){
        cout<<"\n"<<finalResult[i][0]<<"\t\t"<<finalResult[i][1]<<"\t\t"
        <<finalResult[i][2]<<"\t"<<finalResult[i][12]<<"\t\t"<<finalResult[i][13]
        <<"\t\t"<<finalResult[i][14];
    }
    cout<<"\n";

    return 0;
}