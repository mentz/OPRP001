#include <stdio.h>

__device__ const char *STR = "Hello World!";
const char STR_LENGTH = 12;

__global__ void hello() {
    printf("%c\n", STR[threadIdx.x % STR_LENGTH]);
}

int main(void) {
    // int num_threads = STR_LENGTH;
    // int num_blocks = 2;
    // dim3 dimBlock(16,16);
    // dim3  dimGrid(32,32);
    hello<<<1, STR_LENGTH>>>();
    cudaDeviceSynchronize();

    return 0;
}