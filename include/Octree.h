#pragma once

#include "../include/Definitions.h"
#include "../include/PrimitiveTypes.h"
#include "../include/glmafx.h"
#include "../include/VoxelGrid.h"
#include "../include/HashTable.h"
#include "../include/Vec3.h"

#include <vector>
#include <unordered_map>

struct XYZ
{
	U32 x;
	U32 y;
	U32 z;
};

struct Domain
{
	ivec3 minCorner;
	ivec3 voxelSize;
	U32 dim;
	U32 totalVoxelCount;
};

struct Node
{
	ivec3 center; // Center point of octree node (not strictly needed)
	ivec3 voxelSize;
	int density;
};

class Octree
{
public:
	void Build(VoxelGrid & grid);
	void Octree::BuildDomainFromVoxelGrid(VoxelGrid & grid, Domain & domain);
	void BuildLeaves(VoxelGrid & grid, std::unordered_map<U64, Node> & hashTable);
	void BuildBottomUpOctree(VoxelGrid & grid, std::unordered_map<U64, Node> & hashTable, int levelsCount);
	U32 RoundUpToNextPowerOf2(U32 x);
	U64 Part1by4(U32 x);
	U32 Compact4By1(U64 x);
	U64 EncodeMorton4(U32 x, U32 y, U32 z, U32 w);
	U32 GetXFromMorton4(U64 a);
	U32 GetYFromMorton4(U64 a);
	U32 GetZFromMorton4(U64 a);
	U32 GetWFromMorton4(U64 a);
	XYZ GetXYZ(ivec3 point, U32 n);
	void SetLargestVoxelSize(ivec3 size) { m_largestVoxelSize = size; }

	std::unordered_map<U64, Node> m_hashTable;
private:
	Domain m_domain;
	U32 m_maxDepth;
	ivec3 m_largestVoxelSize;
};
