#include <stdio.h>
#include <math.h>
#define N 2047
#define T 1024

__global__ void vecAdd(int *a, int *b, int *c);

int main() {
    int *a, *b, *c;
    int *d_a, *d_b, *d_c;
    
    // initialize a and b with real values (NOT SHOWN)
    
    int size = N * sizeof(int);
    
    cudaMalloc((void**)&d_a, size);
    cudaMalloc((void**)&d_b, size);
    cudaMalloc((void**)&d_c, size);

    a = (int *)malloc(size);
    b = (int *)malloc(size);
    c = (int *)malloc(size);

    for (int i = 0; i < N; i++) {
        a[i] = b[i] = i + 1;
        c[i] = 0;
    }

    cudaMemcpy(d_a, a, size, cudaMemcpyHostToDevice);
    cudaMemcpy(d_b, b, size, cudaMemcpyHostToDevice);

    vecAdd<<<(int)ceil(N/(double)T),T>>>(d_a, d_b, d_c);

    cudaMemcpy(c, d_c, size, cudaMemcpyDeviceToHost);

    cudaFree(d_a);
    cudaFree(d_b);
    cudaFree(d_c);

    printf("c[%*d] = %d\n", (int)log10(N+1), 0, c[0]);
    printf("c[%*d] = %d\n", (int)log10(N+1), N-1, c[N-1]);

    exit(0);
}

__global__ void vecAdd(int *a, int *b, int *c) {
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < N) 
        c[i] = a[i] + b[i];
}