#include <stdio.h>
#include <stdlib.h>
#include <chrono>
#ifdef __APPLE__
#include <OpenCL/cl.h>
#else
#define CL_TARGET_OPENCL_VERSION 100
#include <CL/cl.h>
#endif

#define VECTOR_SIZE 1000

//OpenCL kernel which is run for every work item created.
const char *saxpy_kernel =
"__kernel                                   \n"
"void saxpy_kernel(                         \n"
"                  __global float *A,       \n"
"                  __global float *B,       \n"
"                  __global float *C)       \n"
"{                                          \n"
"    //Get the index of the work-item       \n"
"    int index = get_global_id(0);          \n"
"    C[index] = A[index] * B[index]; \n"
"}                                          \n";


void displayMatrix(int Mat[100][100], int rows, int cols){
    
    for (int i=0; i< rows; i++){
        for (int j=0; j<cols; j++){
            printf("%i", Mat[i][j]);
            printf("\t");
        }
        printf("\n");
    }
}



int main(void) {
  
  double duration_1, duration_2;
  int rows, cols, Mat[100][100], Res[100][100], Res_2[100][100];

  // Allocate space for vectors A, B and C
  float *A = (float*)malloc(sizeof(float)*VECTOR_SIZE);
  float *B = (float*)malloc(sizeof(float)*VECTOR_SIZE);
  float *C = (float*)malloc(sizeof(float)*VECTOR_SIZE);
  for(int i = 0; i < VECTOR_SIZE; i++)
  {
    A[i] = 0;
    B[i] = 0;
    C[i] = 0;
  }

  printf("Enter Matrix Rows:\t");
  scanf("%i", &rows);

  printf("Enter Matrix Colums:\t");
  scanf("%i", &cols);

  srand(time(0));

  for (int i=0; i< rows; i++){
    for (int j=0; j<cols; j++){
      Mat[i][j]=1+rand()%9;
    }
  }

  printf("Random Matrix Generated:\n");
  displayMatrix(Mat, rows, cols);

  auto t1 = std::chrono::system_clock::now();
  for (int i=0; i<rows; i++){
    for (int j=0; j<rows; j++){
      Res[i][j]=0;
      for (int k=0; k<cols; k++){
        Res[i][j] += (Mat[i][k] - Mat[j][k])*(Mat[i][k] - Mat[j][k]);
      }
    }
  }
  auto t2 = std::chrono::system_clock::now();
  duration_1 = std::chrono::duration_cast<std::chrono::nanoseconds>(t2 - t1).count();   

  printf("\nResultant Matrix:\n");
  displayMatrix(Res, rows, rows); 


  // Get platform and device information
  cl_platform_id * platforms = NULL;
  cl_uint     num_platforms;
  
  //Set up the Platform
  cl_int clStatus = clGetPlatformIDs(0, NULL, &num_platforms);
  platforms = (cl_platform_id *)
  malloc(sizeof(cl_platform_id)*num_platforms);
  clStatus = clGetPlatformIDs(num_platforms, platforms, NULL);

  //Get the devices list and choose the device you want to run on
  cl_device_id     *device_list = NULL;
  cl_uint           num_devices;

  clStatus = clGetDeviceIDs( platforms[0], CL_DEVICE_TYPE_CPU, 0,NULL, &num_devices);
  device_list = (cl_device_id *) 
  malloc(sizeof(cl_device_id)*num_devices);
  clStatus = clGetDeviceIDs( platforms[0],CL_DEVICE_TYPE_CPU, num_devices, device_list, NULL);

  int m=0;
  for (int i=0; i<rows; i++){
    for (int j=0; j<rows; j++){
      for (int k=0; k<cols; k++){
        A[m] = abs(Mat[i][k] - Mat[j][k]);
        B[m] = abs(Mat[j][k] - Mat[i][k]);
        m++; 
      }
    }
  }
  
  // Create one OpenCL context for each device in the platform
  cl_context context;
  context = clCreateContext( NULL, num_devices, device_list, NULL, NULL, &clStatus);

  // Create a command queue
  cl_command_queue command_queue = clCreateCommandQueue(context, device_list[0], 0, &clStatus);

  // Create memory buffers on the device for each vector
  cl_mem A_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY,VECTOR_SIZE * sizeof(float), NULL, &clStatus);
  cl_mem B_clmem = clCreateBuffer(context, CL_MEM_READ_ONLY,VECTOR_SIZE * sizeof(float), NULL, &clStatus);
  cl_mem C_clmem = clCreateBuffer(context, CL_MEM_WRITE_ONLY,VECTOR_SIZE * sizeof(float), NULL, &clStatus);

  auto t3 = std::chrono::system_clock::now();

  // Copy the Buffer A and B to the device
  clStatus = clEnqueueWriteBuffer(command_queue, A_clmem, CL_TRUE, 0, VECTOR_SIZE * sizeof(float), A, 0, NULL, NULL);
  clStatus = clEnqueueWriteBuffer(command_queue, B_clmem, CL_TRUE, 0, VECTOR_SIZE * sizeof(float), B, 0, NULL, NULL);

  // Create a program from the kernel source
  cl_program program = clCreateProgramWithSource(context, 1,(const char **)&saxpy_kernel, NULL, &clStatus);

  // Build the program
  clStatus = clBuildProgram(program, 1, device_list, NULL, NULL, NULL);

  // Create the OpenCL kernel
  cl_kernel kernel = clCreateKernel(program, "saxpy_kernel", &clStatus);

  // Set the arguments of the kernel
  clStatus = clSetKernelArg(kernel, 0, sizeof(cl_mem), (void *)&A_clmem);
  clStatus = clSetKernelArg(kernel, 1, sizeof(cl_mem), (void *)&B_clmem);
  clStatus = clSetKernelArg(kernel, 2, sizeof(cl_mem), (void *)&C_clmem);

  // Execute the OpenCL kernel on the list
  size_t global_size = VECTOR_SIZE;           // Process the entire lists
  size_t local_size = VECTOR_SIZE;            // Process one item at a time
  clStatus = clEnqueueNDRangeKernel(command_queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL);

  // Read the cl memory C_clmem on device to the host variable C
  clStatus = clEnqueueReadBuffer(command_queue, C_clmem, CL_TRUE, 0, VECTOR_SIZE * sizeof(float), C, 0, NULL, NULL);

  auto t4 = std::chrono::system_clock::now();
  duration_2 = std::chrono::duration_cast<std::chrono::nanoseconds>(t4 - t3).count(); 

  // Clean up and wait for all the comands to complete.
  clStatus = clFlush(command_queue);
  clStatus = clFinish(command_queue);

  m=0;
  for(int i = 0; i<rows; i++){
    for (int j=0; j<rows; j++){
      for (int k=0; k<cols; k++){
        Res_2[i][j] += C[k+m];
      }
      m=m+cols;
    }
  }

  // Display the result to the screen
  printf("\nResultant Matrix (OpenCL kernel):\n");
  displayMatrix(Res_2, rows, rows);

  printf("Execution time for Method 1 (ns)= %f", duration_1);
  printf("\nExecution time for Method 2 (ns)= %f\n", duration_2);

  // Finally release all OpenCL allocated objects and host buffers.
  clStatus = clReleaseKernel(kernel);
  clStatus = clReleaseProgram(program);
  clStatus = clReleaseMemObject(A_clmem);
  clStatus = clReleaseMemObject(B_clmem);
  clStatus = clReleaseMemObject(C_clmem);
  clStatus = clReleaseCommandQueue(command_queue);
  clStatus = clReleaseContext(context);
  free(A);
  free(B);
  free(C);
  free(platforms);
  free(device_list);
  return 0;
}