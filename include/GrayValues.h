#pragma once

#include "../include/glmafx.h"
#include "../include/PrimitiveTypes.h"
#include <limits>
#include <vector>
#include <unordered_map>


class GrayValues
{
public:
	
	GrayValues();
	GrayValues(std::string file, vec3 voxelSize);
	GrayValues(std::string file, vec3 voxelSize, int extractROI);
	~GrayValues();

	ivec3 GetVoxelSize() { return m_voxelSize; }
	ivec3 GetMinCorner() { return m_minCorner; }
	ivec3 GetMaxCorner() { return m_maxCorner; }
	ivec3 GetDim() { return m_dim; }
	U32 GetVoxelCount() { return m_voxelCount; }
	int GetVoxelDensity(int i) { return m_densityArray[i]; }
	ivec3 GetVoxelCenter(int i) { return m_centerArray[i]; }
	ivec3 * GetCenterArray() { return &m_centerArray[0]; }
	int * GetDensityArray() { return &m_densityArray[0]; }
	int GetDensity(int i) { return m_densityArray[i]; }
	void printAllVoxels();
	bool loadFromFile(std::string filePath, int extractROI);
	long long GetElapsedTime(void) { return m_elapsedTime; }

private:
	ivec3 m_voxelSize;
	ivec3 m_minCorner;
	ivec3 m_maxCorner;
	ivec3 m_dim;
	U64 m_voxelCount;

	std::vector<ivec3> m_centerArray;
	std::vector<int> m_densityArray;
	ivec3 m_centerMid;
	ivec3 m_circleCentre;
	ivec3 m_anterior;
	ivec3 m_posterior;
	ivec3 m_superior;;
	ivec3 m_inferior;
	ivec3 m_ROIMinCorner;
	ivec3 m_ROIMaxCorner;
	ivec3 m_ROICenter;

	void SetROI(void);

	long long m_elapsedTime;

};