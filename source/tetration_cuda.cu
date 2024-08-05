#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include <cuda_runtime.h>

#include "builder/builder.c"
#include "tetration.h"






/**
 *
 *  Main program
 *
 */

#ifndef M_PI_2
#define M_PI_2 1.5707963267948966
#endif

#ifndef M_SQRT2
#define M_SQRT2 1.4142135623730951
#endif

__device__ float modulus(float a, float b) {
    return sqrt(a * a + b * b);
}

__global__ void computeFractal(float* results, sysinfo_t sys_fractal, float width, float height) {
    int idx = blockIdx.x * blockDim.x + threadIdx.x;
    int idy = blockIdx.y * blockDim.y + threadIdx.y;

    if (idx >= width || idy >= height) return;

    float i = sys_fractal.x1 + idx * sys_fractal.pixel_step;
    float j = sys_fractal.y1 + idy * sys_fractal.pixel_step;

    float x = i, y = j, m, p, a, b;
    float angle = 0.0, maxAngle = 0.0;

    m = 0.5 * log(i * i + j * j);
    p = atan2(j, i);
    
    for (int r = 0; r < sys_fractal.iterations && modulus(x, y) < DBL_MAX; r++) {
        
        a = exp(x * m - p * y);
        b = y * m + p * x;
        x = a * cos(b);
        y = a * sin(b);

        angle = fabs(atan(y / x));
        //angle = fabs(atan2(x,y));

        if (r > 0 && angle > maxAngle)
            maxAngle = angle;
    }

    results[idy * (int)width + idx] = maxAngle / (M_PI_2);
}

void checkCudaError(cudaError_t err, const char* msg) {
    if (err != cudaSuccess) {
        fprintf(stderr, "CUDA Error: %s: %s\n", msg, cudaGetErrorString(err));
        exit(EXIT_FAILURE);
    }
}

int main() {
    
    userinfo_t usr_fractal = {0};
    usr_fractal.width = USR_WIDTH * USR_AAF;
    usr_fractal.height = USR_HEIGHT * USR_AAF;
    usr_fractal.center_x = USR_CENTER_X;
    usr_fractal.center_y = USR_CENTER_Y;
    usr_fractal.log_zoom = USR_LOGZOOM;
    usr_fractal.std_diagonal = 10.0 * M_SQRT2;

    float diagonal = usr_fractal.std_diagonal / pow(2.0, usr_fractal.log_zoom);
    float diag_to_width = diagonal / sqrt(1.0 + pow(usr_fractal.height / usr_fractal.width, 2));
    float half_width = diag_to_width * 0.5;
    float half_height = diag_to_width * (usr_fractal.height / usr_fractal.width) * 0.5;

    sysinfo_t sys_fractal = {0};
    sys_fractal.x1 = usr_fractal.center_x - half_width;
    sys_fractal.x2 = usr_fractal.center_x + half_width;
    sys_fractal.y1 = usr_fractal.center_y - half_height;
    sys_fractal.y2 = usr_fractal.center_y + half_height;
    sys_fractal.pixel_step = diag_to_width / usr_fractal.width;
    sys_fractal.iterations = ITERATIONS;

    int total_pixels = (int)(usr_fractal.width * usr_fractal.height);
    
    float* results = (float*)malloc(total_pixels * sizeof(float));
    if (results == NULL) {
        fprintf(stderr, "Failed to allocate host memory.\n");
        return EXIT_FAILURE;
    }
    
    
    
    // Nvidia CUDA specific commands

    float* d_results;
    checkCudaError(cudaMalloc(&d_results, total_pixels * sizeof(float)), "Allocating device memory");

    int N = 32;  // Choose a reasonable block size
    dim3 blockDim(N, N);
    dim3 gridDim((int)ceil(usr_fractal.width / (float)N), (int)ceil(usr_fractal.height / (float)N));

    computeFractal<<<gridDim, blockDim>>>(d_results, sys_fractal, usr_fractal.width, usr_fractal.height);
    checkCudaError(cudaGetLastError(), "Launching kernel");
    checkCudaError(cudaDeviceSynchronize(), "Synchronizing after kernel");

    checkCudaError(cudaMemcpy(results, d_results, total_pixels * sizeof(float), cudaMemcpyDeviceToHost), "Copying results to host");

    cudaFree(d_results);
    
    
    
    // Back to normal C
    
    builder_DataBuild(usr_fractal, results, BUILD_AAF, BUILD_COLORMAP);
    system("start " FILE_BITMAP);
    
    FILE* fp = fopen(FILE_RAWOUT, "wb");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open file for writing.\n");
        return EXIT_FAILURE;
    }
    
    fwrite(&usr_fractal, sizeof(userinfo_t), 1, fp);
    fwrite(results, sizeof(float), total_pixels, fp);
    fclose(fp);
    free(results);

    return 0;
}
