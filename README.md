# Parallel Algorithms for the Analysis and Synthesis of Data

Commands to compile:
g++ Task1.cpp -o Task1 -fopenmp -lpthread
g++ Task2.cpp -o Task2 -fopenmp -lpthread
gcc -I /opt/intel/system_studio_2020/opencl-sdk/include -L /opt/intel/system_studio_2020/opencl-sdk/lib64 Task3.cpp -o Task3 -lOpenCL