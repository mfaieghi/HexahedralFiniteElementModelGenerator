#include "../include/Octree.h"

#include <iostream>
#include <bitset>

void Octree::Build(VoxelGrid & grid)
{
	BuildDomainFromVoxelGrid(grid, m_domain);
	m_maxDepth = glm::log(m_domain.totalVoxelCount) / glm::log(8);
	
	
	ivec3 levelsCount = static_cast<ivec3>(glm::log2(static_cast<vec3>(m_largestVoxelSize) / static_cast<vec3>(m_domain.voxelSize)));
	if (levelsCount.x != levelsCount.y && levelsCount.x != levelsCount.z)
	{
		printf("Error in setting the largest voxel size in the octree\n");
		exit(1);
	}
	else
	{
		BuildLeaves(grid, m_hashTable);

		int i = 1;
		while (levelsCount.x >= i)
		{
			BuildBottomUpOctree(grid, m_hashTable, i);
			++i;
		}
	}
}

//Transfer the field data format into a grid
//The main difference is that field is a compact data representation that only contain
//non-zero voxels, but the grid allocates space for every voxel whether full or empty
void Octree::BuildDomainFromVoxelGrid( VoxelGrid & grid, Domain & domain)
{
	//Octree domain
	//In order to configure the data in a octree, the domain has to be a power of 2.
	U32 dimDomain = glm::max(grid.m_dim.x, glm::max(grid.m_dim.y, grid.m_dim.z));
	domain.minCorner = grid.m_minCornerFirstVoxel;
	domain.voxelSize = grid.m_voxelSize;
	domain.dim = RoundUpToNextPowerOf2(dimDomain);
	domain.totalVoxelCount = glm::pow(dimDomain, 3);
}

//Create the last node before leaves
//One way of populating the octree was to set every voxel as a node that does not 
//have any children. Instead, here we populate the last layer the nodes in a way
//that their children are the voxels. This will decrease one layer of populating
//the octree. To do so, we have find the voxel and domain size of the nodes of
//this layer.
void Octree::BuildLeaves( VoxelGrid & grid, std::unordered_map<U64, Node> & hashTable)
{
	ivec3 minCorner = m_domain.minCorner;
	U32 levelLeaves = m_maxDepth;
	ivec3 voxelSizeLeaves = m_domain.voxelSize;
	U32 dimLeaves = m_domain.dim;
	U32 l = log2(dimLeaves);
	int intMin = std::numeric_limits<int>::min();

	//for ( auto & voxel : grid.m_data)
	for (U32 i = 0; i < grid.m_densityArray.size(); ++i)
	{
		int density = grid.m_densityArray[i];
		if (density != intMin)
		{
			//Find the center of each voxel in the last node layer coords
			//ivec3 point = (voxel.center - minCorner) / voxelSizeLeaves;
			//U32 idxLeaves = point.x + (point.y * dimLeaves) + (point.z * dimLeaves * dimLeaves);
			ivec3 xyz = grid.GetGridCoordsFromIndex(i);
			//Find the center of each node in the last layer
			//U32 zLeaves = i / (dimLeaves * dimLeaves);
			//U32 yLeaves = (i - zLeaves * dimLeaves * dimLeaves) / dimLeaves;
			//U32 xLeaves = i - zLeaves * dimLeaves * dimLeaves - yLeaves * dimLeaves;
			U32 zLeaves = static_cast<U32>(xyz.z);
			U32 yLeaves = static_cast<U32>(xyz.y);
			U32 xLeaves = static_cast<U32>(xyz.x);
			U64 keyLeaves = EncodeMorton4(xLeaves, yLeaves, zLeaves, levelLeaves);
			ivec3 center = minCorner + ivec3(xLeaves, yLeaves, zLeaves) * voxelSizeLeaves + voxelSizeLeaves / 2;
			Node newNode{ center, voxelSizeLeaves, density };
			if (m_hashTable.count(keyLeaves) > 0)
			{
				printf("here\n");
			}
			m_hashTable.emplace(keyLeaves, newNode);
		}
	}
}

void Octree::BuildBottomUpOctree( VoxelGrid & grid, std::unordered_map<U64, Node> & hashTable, int levelsCount)
{
	ivec3 minCorner = m_domain.minCorner;
	U32 levelNode = m_maxDepth - levelsCount;
	U32 levelChild = levelNode + 1;
	U32 levelDifferenceFromBottom = m_maxDepth - levelNode;
	ivec3 voxelSizeNode = m_domain.voxelSize * (1 << levelDifferenceFromBottom);
	ivec3 voxelSizeChild = voxelSizeNode >> 1;
	ivec3 halfVoxelSizeChild = voxelSizeChild >> 1;
	U32 dimNode = m_domain.dim / (1 << levelDifferenceFromBottom);
	U32 dimChild = dimNode << 1;
	U32 n = log2(dimNode);
	U32 c = log2(dimChild);

	std::vector<U64> key8Children;

	for (int i = 0; i < dimNode; ++i)
	{
		for (int j = 0; j < dimNode; ++j)
		{
			for (int k = 0; k < dimNode; ++k)
			{
				//Parent node location
				ivec3 cNode = minCorner + ivec3(i, j, k) * voxelSizeNode + voxelSizeChild;
				int densitySum{ 0 };

				//Find all the eight possible children of the the given node
				//See if they exist in the octree (hash map container)
				//As soon as one child detected not to be in the container, exit to the next parent node
				//If all the eight children of the given node are part of the octree, proceed.
				for (U32 l = 0; l < 8; ++l)
				{
					ivec3 offset;
					offset.x = ((l & 1) ? halfVoxelSizeChild.x : -halfVoxelSizeChild.x);
					offset.y = ((l & 2) ? halfVoxelSizeChild.y : -halfVoxelSizeChild.y);
					offset.z = ((l & 4) ? halfVoxelSizeChild.z : -halfVoxelSizeChild.z);
					ivec3 cChild = cNode + offset;
					ivec3 pointChild = (cChild - minCorner) / voxelSizeChild;
					//Vec3 aPointChild = { pointChild.x, pointChild.y, pointChild.z };
					U32 idxChild = pointChild.x + (pointChild.y * dimChild) + (pointChild.z * dimChild * dimChild);
					U32 zChild = idxChild / (dimChild * dimChild);
					U32 yChild = (idxChild - zChild * dimChild * dimChild) / dimChild;
					U32 xChild = idxChild - zChild * dimChild * dimChild - yChild * dimChild;
					U64 keyChild = EncodeMorton4(xChild, yChild, zChild, levelChild);
					if (m_hashTable.count(keyChild) != 1)
					{
						key8Children.clear();
						break;
					}
					else
					{
						key8Children.push_back(keyChild);
						densitySum += m_hashTable[keyChild].density;
					}

				}
				//Add the parent node and destroy the descendant nodes
				if (key8Children.size() == 8)
				{
					ivec3 pointNode = (cNode - minCorner) / voxelSizeNode;
					//Vec3 aPointNode = { pointNode.x, pointNode.y, pointNode.z };
					U32 idxNode = pointNode.x + (pointNode.y * dimNode) + (pointNode.z  * dimNode * dimNode);
					U32 zNode = idxNode / (dimNode * dimNode);
					U32 yNode = (idxNode - zNode * dimNode * dimNode) / dimNode;
					U32 xNode = idxNode - zNode * dimNode * dimNode - yNode * dimNode;
					U64 keyNode = EncodeMorton4(xNode, yNode, zNode, levelNode);
					Node newNode{ cNode, voxelSizeNode, densitySum / 8 };
					m_hashTable.emplace(keyNode, newNode);
					for ( auto aKey : key8Children)
					{
						m_hashTable.erase(aKey);
					}
					key8Children.clear();
				}
			}
		}
	}
}

U32 Octree::RoundUpToNextPowerOf2(U32 x)
{
	--x;
	x |= x >> 1;
	x |= x >> 2;
	x |= x >> 4;
	x |= x >> 8;
	x |= x >> 16;
	++x;
	return x;
}

U64 Octree::Part1by4(U32 x)
{
	U64 a = static_cast<uint64_t>(x);
	a &= 0XFFFF;
	a = (a | (a << 24)) & 0XFF000000FF;
	a = (a | (a << 12)) & 0XF000F000F000F;
	a = (a | (a << 6)) & 0X303030303030303;
	a = (a | (a << 3)) & 0X1111111111111111;
	return a;
}

U32 Octree::Compact4By1(U64 x)
{
	x &= 0X1111111111111111;
	x = (x | x >> 3) & 0X303030303030303;
	x = (x | x >> 6) & 0XF000F000F000F;
	x = (x | x >> 12) & 0XFF000000FF;
	x = (x | x >> 24) & 0XFFFF;
	U32 a = static_cast<U32>(x);
	return a;
}
U64 Octree::EncodeMorton4(U32 x, U32 y, U32 z, U32 w)
{
	return (Part1by4(w) << 3) + (Part1by4(z) << 2) + (Part1by4(y) << 1) + Part1by4(x);
}

U32 Octree::GetXFromMorton4(U64 a)
{
	return Compact4By1(a);
}

U32 Octree::GetYFromMorton4(U64 a)
{
	return Compact4By1(a >> 1);
}

U32 Octree::GetZFromMorton4(U64 a)
{
	return Compact4By1(a >> 2);
}

U32 Octree::GetWFromMorton4(U64 a)
{
	return Compact4By1(a >> 3);
}

XYZ Octree::GetXYZ(ivec3 point, U32 n)
{
	int idx = point.x + (point.y << n) + (point.z << 2 * n);
	U32 z = idx >> (2 * n);
	idx -= z >> (2 * n);
	U32 y = idx >> n;
	U32 x = idx & ((1 << n) - 1);
	return{ x, y, z };
}