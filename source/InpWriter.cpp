#include "../include/InpWriter.h"
#include "../include/HashTable.h"
#include "../include/Definitions.h"

#include <fstream>
#include <sstream>
#include <vector>

void exportINP(std::string name, vec3 outputVoxelSize, U64 filledVoxelCount, Vec3 * verticesArray, int * materialsArray, int materialsIndexArray, U64 numberOfMaterials)
{

	//Unify vertices
	std::unordered_map<Vec3, size_t> uniqueVertices{};
	std::vector<Vec3> vertices;
	std::vector<U64> indices;

	for (U64 i = 0; i < filledVoxelCount * 8; i++)
	{
		Vec3 vertex = verticesArray[i];
		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = vertices.size();
			indices.push_back(vertices.size());
			vertices.push_back(vertex);
		}
		else
		{
			indices.push_back(uniqueVertices.at(vertex));
		}
	}

	std::stringstream stream;
	stream << outputVoxelSize.x << ", " << outputVoxelSize.y << ", " << outputVoxelSize.z;
	std::string fileName = "FEA_Model_" + stream.str() + "mm_" + name + ".inp";
	std::ofstream file;
	file.open(fileName.c_str());
	file << "*HEADING\n";
	file << "Written by FEA-Modeller-CUDA\n";
	file << "Units: mm\n\n";
	file << "PART: name=" << name << "\n\n";

	//Write nodes
	file << "*NODE\n";
	file.precision(PRECISION + 2);
	for (U64 i = 0; i < vertices.size(); i++)
	{
		float x = static_cast<float>(vertices[i].x);
		float y = static_cast<float>(vertices[i].y);
		float z = static_cast<float>(vertices[i].z);
		float den = pow(10, PRECISION);
		x /= den;
		y /= den;
		z /= den;
		file << i + 1 << ", " << x << ", " << y << ", " << z << std::endl;
	}

	//Write elements
	file << "*ELEMENT, TYPE=C3D8\n";
	U64 j = 1;
	for (unsigned int i = 0; i < indices.size(); i += 8)
	{
		file << j++ << ", ";
		file << indices[i] + 1 << ", ";
		file << indices[i + 1] + 1 << ", ";
		file << indices[i + 2] + 1 << ", ";
		file << indices[i + 3] + 1 << ", ";
		file << indices[i + 4] + 1 << ", ";
		file << indices[i + 5] + 1 << ", ";
		file << indices[i + 6] + 1 << ", ";
		file << indices[i + 7] + 1 << std::endl;
	}
	file << "\n";

	//Which materials assigned to which element
	for (unsigned int i = 0; i < indices.size(); i++)
	{
		file << "*Elset, elset = Set_" << i << "\n";
		file << i << "\n";
		file << "*Solid Section, elset = Set_" << i << ", material = Mat_" << indices[i] << "\n";
	}
	file << "\n";

	//Write materials
	for (unsigned int i = 0; i < numberOfMaterials; i++)
	{
		file << "**MATERIALS\n";
		file << "*Material, name = Mat_" << i << "\n";
		file << "*Elastic\n";
		file << materialsArray[i] << ", " << 0.3 << "\n";
	}

	printf("Writing file is completed.\n");
}