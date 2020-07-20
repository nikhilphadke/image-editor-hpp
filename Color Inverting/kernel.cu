#include<cuda.h>
#include <cuda_runtime.h>
#include <device_launch_parameters.h>
#include <math.h>
#include <stdio.h>
#include "helper_timer.h"

void CleanUp();


__global__ void ColorInvert(float *d_in,float *d_out)
{
	// variable declarations
	int row = blockIdx.x * blockDim.x + threadIdx.x;
	// code
	if (row < 6162980 && (row % 3 == 0))
	{
		const float red = d_in[row];
		const float green = d_in[row + 1];
		const float blue = d_in[row + 2];

			d_out[row]   = 255.0f-red;
			d_out[row+1] = 255.0f-green;
			d_out[row+2] = 255.0f-blue;

	}

}


float *d_in=NULL;
float *d_out=NULL;

void ProcessImage(float *rgb,float *x1,float *timeonGpu, char deviceName[])
{

	cudaError_t err;

	cudaDeviceProp dev_prop;
	cudaGetDeviceProperties(&dev_prop, 0);


	err = cudaMalloc((void **)&d_in, sizeof(float) * 6162980);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s At Line No. %d.\nExitting ...\n", cudaGetErrorString(err), __FILE__, __LINE__);

		exit(EXIT_FAILURE);
	}

	err = cudaMalloc((void **)&d_out, sizeof(float) * 6162980);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s At Line No. %d.\nExitting ...\n", cudaGetErrorString(err), __FILE__, __LINE__);

		exit(EXIT_FAILURE);
	}

	err = cudaMemcpy(d_out, x1, sizeof(float) * 6162980, cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s At Line No. %d.\nExitting ...\n", cudaGetErrorString(err), __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

    //MessageBox(NULL,TEXT("After Mem Alloc"),TEXT("Here"),MB_OK);
	err = cudaMemcpy(d_in, rgb, sizeof(float) * 6162980, cudaMemcpyHostToDevice);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s At Line No. %d.\nExitting ...\n", cudaGetErrorString(err), __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	sprintf(deviceName, "%s",dev_prop.name);

	dim3 DimGrid = dim3(ceil((double) 6162980/1024),1, 1);
	dim3 DimBlock = dim3(1024, 1, 1);

	// start timer
    StopWatchInterface *timer = NULL;
	sdkCreateTimer(&timer);
	sdkStartTimer(&timer);

	ColorInvert<<<DimGrid, DimBlock>>>(d_in,d_out);

	sdkStopTimer(&timer);
	*timeonGpu = sdkGetTimerValue(&timer);
	sdkDeleteTimer(&timer);

	err = cudaMemcpy(x1, d_out, sizeof(float) * 6162980, cudaMemcpyDeviceToHost);
	if (err != cudaSuccess)
	{
		printf("GPU Memory Fatal Error = %s In File Name %s At Line No. %d.\nExitting ...\n", cudaGetErrorString(err), __FILE__, __LINE__);
		exit(EXIT_FAILURE);
	}

	CleanUp();

}


void CleanUp()
{
	if (d_in)
	{
		cudaFree(d_in);
		d_in = NULL;
	}


	if (d_out)
	{
		cudaFree(d_out);
		d_out = NULL;
	}


}