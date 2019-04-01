#pragma once
#include "../include/glmafx.h"
#include "../include/PrimitiveTypes.h"
#include "../include/GrayValues.h"
#include "../include/Definitions.h"
#include <iostream>
#include <chrono>

class VoxelGrid
{
public:
	VoxelGrid();
	VoxelGrid(VoxelGrid in, vec3 desiredVoxelSize, int shouldSmooth);
	VoxelGrid(GrayValues image, vec3 voxelSize);
	//VoxelGrid(vec3 minCorner, vec3 maxMargin, vec3 voxelSize);
	~VoxelGrid();

	ivec3 GetVoxelSize() { return m_voxelSize; }
	ivec3 GetMinCorner() { return m_minCorner; }
	ivec3 GetMaxCorner() { return m_maxCorner; }
	ivec3 GetFirstVoxelCenter() { return m_firstVoxelCenter; }
	ivec3 GetDim() { return m_dim; }
	U64 GetTotalVoxelCount() { return m_totalVoxelCount; }
	U64 GetFilledVoxelCount() { return m_filledVoxelCount; }
	unsigned int * GetDensityArray() { return m_densityArray; }
	unsigned int GetVoxelDensity(U64 i) { return m_densityArray[i]; }
	ivec3 GetGridCoordsFromPoint(ivec3 point);
	ivec3 GetGridCoordsFromIndex(U64 index);
	ivec3 GetVoxelFristVertexFromIndex(U64 index);
	ivec3 GetVoxelCenter(U64 index);
	U64 GetIndexFromGridCoords(ivec3 gridCoords);
	void SetVoxelDensity(U64 index, int value) { m_densityArray[index] = value; }
	void SetVoxelSize(vec3 size) { m_voxelSize = size; }
	void SetMinCorner(vec3 corner) { m_minCorner = corner; }
	void SetMaxCorner(vec3 corner) { m_maxCorner = corner; }
	void SetFirstVoxelCenter(vec3 center) { m_firstVoxelCenter = center; }
	void SetDim(ivec3 dim) { m_dim = dim; }
	void SetTotalVoxelCount(U64 number) { m_totalVoxelCount = number; }
	void SetFilledVoxelCount(U64 number) { m_filledVoxelCount = number; }
	void SetDensityArrayMemory(U64 number) { m_densityArray = new unsigned int[number]; }

	long long GetElapsedTime(void) { return m_elapsedTime; }

private:
	ivec3 m_voxelSize;
	ivec3 m_minCorner;
	ivec3 m_maxCorner;
	ivec3 m_firstVoxelCenter;
	ivec3 m_dim;
	U64 m_totalVoxelCount;
	U64 m_filledVoxelCount;
	unsigned int * m_densityArray;

	long long m_elapsedTime;

};
