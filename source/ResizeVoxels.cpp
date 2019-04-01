#include "../include/ResizeVoxels.h"
#include "../include/Definitions.h"

#include <iostream>
#include <ctime>
#include <cmath>

int VoxelsOverlapTest(ivec3 v1, ivec3 s1, ivec3 v2, ivec3 s2)
{
	float t;
	if ((t = v1.x - v2.x) >= s2.x || -t >= s1.x) return 0;
	if ((t = v1.y - v2.y) >= s2.y || -t >= s1.y) return 0;
	if ((t = v1.z - v2.z) >= s2.z || -t >= s1.z) return 0;
	return 1;
}


void KERNEL_ResizeVoxels(U32 threadsCount, ivec3 inVoxelSize, ivec3 inMinCorner, ivec3 inDim, int * inDensityArray,
	ivec3 outVoxelSize, ivec3 outMinCorner, ivec3 outDim, int * outDensityArray, U32 &outFilledVoxelCount)
{
	for(U32 i = 0; i < threadsCount; i++)
	{
		//printf("thread no. %d: inVoxelSize: %f, %f, %f\n", i, inVoxelSize.x, inVoxelSize.y, inVoxelSize.z);
		//printf("thread no. %d: inMinCorner: %f, %f, %f\n", i, inMinCorner.x, inMinCorner.y, inMinCorner.z);
		//printf("thread no. %d: inDim: %d, %d, %d\n", i, inDim.x, inDim.y, inDim.z);
		//printf("thread no. %d: outVoxelSize: %f, %f, %f\n", i, outVoxelSize.x, outVoxelSize.y, outVoxelSize.z);
		//printf("thread no. %d: outMinCorner: %f, %f, %f\n", i, outMinCorner.x, outMinCorner.y, outMinCorner.z);
		//printf("thread no. %d: outDim: %d, %d, %d\n", i, outDim.x, outDim.y, outDim.z);
		//printf("i: %d\n", i);
		int limit = std::numeric_limits<int>::min();
		int inDensity = inDensityArray[i];
		if (inDensity != limit)
		{
			U32 z = i / (inDim.x * inDim.y);
			U32 y = (i - z * inDim.x * inDim.y) / inDim.x;
			U32 x = i - z * inDim.x * inDim.y - y * inDim.x;
			//printf("thread no. %d: inGridCoords: %d, %d, %d\n", i, x, y, z);
			ivec3 inVoxelFirstVertex = inMinCorner + ivec3(x,y,z) * inVoxelSize;
			//printf("thread no. %d: inVoxelFirstVertex: %f, %f, %f\n", i, inVoxelFirstVertex.x, inVoxelFirstVertex.y, inVoxelFirstVertex.z);

			///If inVoxelSize is N times greater than outVoxelSize, then 
			///outMinLocalID will represent N * i, while
			///outMaxLocalID will represent N * (i + 1)

			ivec3 N = inVoxelSize / outVoxelSize;
			///LEGACY: ivec3 outMinLocalID = static_cast<ivec3>((inVoxelFirstVertex - outMinCorner) / outVoxelSize);
			ivec3 outMinLocalID = ivec3(x, y, z) * N;

			///LEGACY: ivec3 outMaxLocalID = static_cast<ivec3>((inVoxelFirstVertex + inVoxelSize - outMinCorner) / outVoxelSize) - ivec3(1);
			ivec3 outMaxLocalID = ivec3(x + 1, y + 1, z + 1) * N;


			
			//printf("thread no. %d: outMinLocalID: %d, %d, %d\n", i, outMinLocalID.x, outMinLocalID.y, outMinLocalID.z);
			//printf("thread no. %d: outMaxLocalID: %d, %d, %d\n", i, outMaxLocalID.x, outMaxLocalID.y, outMaxLocalID.z);

			for (U32 j = outMinLocalID.z; j < outMaxLocalID.z; j++)
			{
				for (U32 k = outMinLocalID.y; k < outMaxLocalID.y; k++)
				{
					for (U32 l = outMinLocalID.x; l < outMaxLocalID.x; l++)
					{
						U32 outVoxelIndex = { static_cast<U32>(l) + outDim.x * static_cast<U32>(k) + outDim.x * outDim.y * static_cast<U32>(j) };
						//printf("thread no. %d: outVoxelIndex: %d\n", i, outVoxelIndex);
						if (outDensityArray[outVoxelIndex] != inDensity)
						{
							ivec3 outVoxelFirstVertex = outMinCorner + ivec3(l, k, j) * outVoxelSize;
							//if (outVoxelIndex == 4)
							//{
							//	printf("hit4 -- thread no. %d\n", i);
							//}
							//printf("thread no. %d: outVoxelFirstVertex: %f, %f, %f\n", i, outVoxelFirstVertex.x, outVoxelFirstVertex.y, outVoxelFirstVertex.z);
							//printf("thread no. %d: outVoxelIndex: %d\n", i, outVoxelIndex);
							if (VoxelsOverlapTest(outVoxelFirstVertex, outVoxelSize, inVoxelFirstVertex, inVoxelSize))
							{
								outDensityArray[outVoxelIndex] = inDensity;
								//printf("thread no. %d: outVoxelIndex: %d\n", i, outVoxelIndex);s
								outFilledVoxelCount++;
							}
						}
					}
				}
			}
		}
	}
}


void ResizeVoxels(VoxelGrid & in, VoxelGrid & out)
{
	U32 threadsCount = in.GetTotalVoxelCount();
	
	ivec3 h_inVoxelSize = in.GetVoxelSize();
	
	ivec3 h_inMinCorner = in.GetMinCorner();
	
	ivec3 h_inDim = in.GetDim();

	int * h_inDensityArray = new int[in.GetTotalVoxelCount()];
	for (U32 i = 0; i < in.GetTotalVoxelCount(); i++)
	{
		h_inDensityArray[i] = in.GetVoxelDensity(i);
		//printf("===thread no. %d -->inDensityArray[i]: %d\n", i, inDensityArray[i]);
	}

	ivec3 h_outVoxelSize = out.GetVoxelSize();
											
	ivec3 h_outMinCorner = out.GetMinCorner();

	ivec3 h_outDim = out.GetDim();

	int * h_outDensityArray = new int[out.GetTotalVoxelCount()];
	memset(h_outDensityArray, std::numeric_limits<int>::min(), sizeof(int) * out.GetTotalVoxelCount());
	
	U32 h_outFilledVoxelCount = 0;
	
	//Launch kerenl
	clock_t start = clock();
	KERNEL_ResizeVoxels(threadsCount, h_inVoxelSize, h_inMinCorner, h_inDim, h_inDensityArray, h_outVoxelSize,
		h_outMinCorner, h_outDim, h_outDensityArray, h_outFilledVoxelCount);
	clock_t end = clock();
	clock_t elapsedTime = end - start;
	printf("Resizing voxels is done!\n");

	//Retrieve Results
	for (U32 i = 0; i < out.GetTotalVoxelCount(); i++)
	{
		out.SetVoxelDensity(i, h_outDensityArray[i]);
		//printf("===thread no. %d --> %d\n", i, outDensityArray[i]);
	}
	out.SetFilledVoxelCount(h_outFilledVoxelCount);
}