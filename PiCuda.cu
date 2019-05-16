#include <stdio.h>
#include <cuda_runtime.h>

#define ITER  10000000000  // Number of bins
#define NUMBLOCKS   13  // Number of thread blocks
#define NUMTHREADS 192  // Number of threads per block
int tid;
float pi;

// Kernel
__global__ void pic(float *sum, int nbin, float step, int nthreads, int nblocks) {
	int i;
	float x;
	int idx = blockIdx.x*blockDim.x+threadIdx.x;  // Sequential thread index across the blocks
	for (i=idx; i< nbin; i+=nthreads*nblocks) {
		x = (i+0.5)*step;
		sum[idx] += 4.0/(1.0+x*x);
	}
}

// Main
int main() {
	dim3 dimGrid(NUMBLOCKS,1,1);  // Grid dimensions
	dim3 dimBlock(NUMTHREADS,1,1);  // Block dimensions

	float *sumHost, *sumDev;  // Pointer to host & device arrays

	float step = 1.0/ITER;  // Step size

	size_t size = NUMBLOCKS*NUMTHREADS*sizeof(float);  //Array memory size

	sumHost = (float *)malloc(size);  //  Allocate array on host

	cudaMalloc((void **) &sumDev, size);  // Allocate array on device
	// Initialize array in device to 0
	cudaMemset(sumDev, 0, size);
	// Do calculation on device
	pic <<<dimGrid, dimBlock>>> (sumDev, ITER, step, NUMTHREADS, NUMBLOCKS); // call CUDA kernel
	// Retrieve result from device and store it in host array
	cudaMemcpy(sumHost, sumDev, size, cudaMemcpyDeviceToHost);
	for(tid=0; tid<NUMTHREADS*NUMBLOCKS; tid++)
		pi += sumHost[tid];
	pi *= step;

	// Print results
	printf("PI = %f\n",pi);

	// Cleanup
	free(sumHost);
	cudaFree(sumDev);

	return 0;
}
