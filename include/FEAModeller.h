#pragma once

#include "../include/GrayValues.h"
#include "../include/VoxelGrid.h"
#include "../include/Vec3.h"
#include "../include/Definitions.h"
#include "../include/CreateHexahedral.h"
#include "../include/HashTable.h"
//#include "../include/Octree.h"
//#include "../include/VTKVoxelRenderer.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <limits>
#include <map>
#include <algorithm>
#include <chrono>
#include <string>
#include <map>
#include <unordered_map>
#include <fstream>

class FEAModeller
{
public:
	FEAModeller();
	~FEAModeller();
	//void Resize();
	void RemoveDuplicateVertices();
	void CreateHexas();
	float CalculateMaterial(int density);
	void ChangeMaterialResolution(float min, float max, float stepSize);
	void RemoveDuplicateMaterials();
	void ProcessMaterials();
	void ExportINP();
	void ExportTXT();
	//void Render(int render, int writeFile);
	U64 GetProblemSize(void) { return m_problemSize; }
	std::string GetFileName(void) { return m_fileName; }
	long long GetFileReadingElapsedTime(void) { return m_image.GetElapsedTime(); };
	
private:
	std::string m_fileName;
	std::string m_filePath;
	vec3 m_inputVoxelSize;
	vec3 m_outputVoxelSize;
	vec3 m_largestVoxelSize;
	GrayValues m_image;
	VoxelGrid m_inputGrid;
	VoxelGrid m_outputGrid;
	//Vec3 * m_verticesArray;
	std::vector<Vec3> m_verticesArray;
	std::vector<Vec3> m_nodes;
	std::vector<U64> m_nodesIndices;
	std::vector<int> m_materials;
	std::vector<U64> m_materialsIndices;
	int m_integrateNodesInfo;
	int m_integrateMaterials;
	float m_slope;
	float m_intercept;
	float m_poissonRatio;
	float m_stepSize;
	float m_minMaterial;
	float m_maxMaterial;
	U64 m_elementsCount;
	std::unordered_multimap<float, int> m_materialsPool;
	U64 m_problemSize;
	int m_useOctree;
	//Octree m_octree;
	std::ofstream m_detailedLogFile;
};
