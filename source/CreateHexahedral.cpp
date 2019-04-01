#include "../include/CreateHexahedral.h"
#include "../include/Definitions.h"
#include "../include/PrimitiveTypes.h"
#include "../include/Vec3.h"

#include <stdio.h>
#include <ctime>

void KERNEL_CreateHexahedral(U64 threadsCount, Vec3 voxelSize, std::vector<Vec3> firstVertexArray, std::vector<Vec3> & verticesArray)
{
	for(U64 i = 0; i < threadsCount; i++)
	{
		Vec3 firstVertex = firstVertexArray[i];
		//printf("thread no. %d --> %f %f, %f\n", i, firstVertex.x, firstVertex.y, firstVertex.z);

		Vec3 step = {0.0f, 0.0f, 0.0f};
		
		//v0
		//verticesArray[8 * i] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);
		//printf("thread no. %d --> %f %f, %f\n", i, verticesArray[8 * i].x, verticesArray[8 * i].y, verticesArray[8 * i].z);

		//v1
		step.x = voxelSize.x;
		//verticesArray[8 * i + 1] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);

		//v2
		step.z = - voxelSize.z;
		//verticesArray[8 * i + 2] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);

		//v3
		step.x = 0.0f;
		//verticesArray[8 * i + 3] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);

		//v4
		step.z = 0.0f;
		step.y = voxelSize.y;
		//verticesArray[8 * i + 4] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);

		//v5
		step.x = voxelSize.x;
		//verticesArray[8 * i + 5] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);
		
		//v6
		step.z = - voxelSize.z;
		//verticesArray[8 * i + 6] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);

		//v7
		step.x = 0.0f;
		//verticesArray[8 * i + 7] = firstVertex + step;
		verticesArray.push_back(firstVertex + step);
	}
}


void CreateHexahedral(VoxelGrid & grid, std::vector<Vec3> & a_verticesArray)
{

	U64 threadsCount = grid.GetFilledVoxelCount();

	Vec3 h_voxelSize;
	h_voxelSize.x = grid.GetVoxelSize().x;
	h_voxelSize.y = grid.GetVoxelSize().y;
	h_voxelSize.z = grid.GetVoxelSize().z;
	
	std::vector<Vec3> h_firstVertexArray;
	int limit = std::numeric_limits<int>::min();
	for (U64 i = 0; i < grid.GetTotalVoxelCount(); i++)
	{
		if(grid.GetVoxelDensity(i) != limit)
		{
			ivec3 dim = grid.GetDim();
			U64 z = i / (dim.x * dim.y);
			U64 y = (i - z * dim.x * dim.y) / dim.x;
			U64 x = i - z * dim.x * dim.y - y * dim.x;
			ivec3 firstVertex = grid.GetMinCorner() + ivec3(x, y, z) * grid.GetVoxelSize();
			Vec3 point;
			point.x = firstVertex.x;
			point.y = firstVertex.y;
			point.z = firstVertex.z;		
			h_firstVertexArray.push_back(point);
		}
	}

	//Vec3 * h_verticesArray = new Vec3[8 * grid.GetFilledVoxelCount()];
	std::vector<Vec3> h_verticesArray;
	h_verticesArray.reserve(8 * grid.GetFilledVoxelCount());

	//Launch kerenl
	clock_t start = clock();
	KERNEL_CreateHexahedral(threadsCount, h_voxelSize, h_firstVertexArray, h_verticesArray);
	clock_t end = clock();
	clock_t elapsedTime = end - start;
	printf("Creating hexahedrals is done!\n");

	//Retrieve Results
	//memcpy(a_verticesArray, h_verticesArray, sizeof(Vec3) * 8 * grid.GetFilledVoxelCount());
	for (U64 i = 0; i < h_verticesArray.size(); ++i)
	{
		a_verticesArray.push_back(h_verticesArray[i]);
	}

	//Clean up
	h_firstVertexArray.clear();
}