#include "../include/FEAModeller.h"

FEAModeller::FEAModeller()
	: m_fileName{ INPUT_FILE_NAME }
	, m_filePath{ "..//FEAModeller-C//model//" }
	, m_inputVoxelSize{ VOXEL_SIZE_X, VOXEL_SIZE_Y, VOXEL_SIZE_Z }
	, m_outputVoxelSize{ DESIRED_VOXEL_SIZE_X, DESIRED_VOXEL_SIZE_Y, DESIRED_VOXEL_SIZE_Z }
	, m_slope{MATERIAL_SLOPE }
	, m_intercept{MATERIAL_INTERCEPT }
	, m_poissonRatio{ MATERIAL_POISSION_RATIO }
	, m_stepSize{MATERIAL_STEP_SIZE}
	, m_integrateNodesInfo{ INTEGRATE_NODE_INFO }
	, m_integrateMaterials{INTEGRATE_MATERIAL}
	, m_largestVoxelSize{ m_inputVoxelSize * pow(2.0f, VOXEL_GROWTH_POWER) }
	, m_image{ m_filePath + m_fileName + ".txt", m_inputVoxelSize }
	, m_inputGrid{ m_image, m_inputVoxelSize }
	, m_outputGrid{ m_inputGrid, m_outputVoxelSize, 1 }
	, m_useOctree{ OCTREE }
{
	//m_octree.SetLargestVoxelSize(m_largestVoxelSize * pow(10.0f, PRECISION));
	m_problemSize = m_image.GetVoxelCount();
	std::string fileName = "detailedLog_" + m_fileName + ".txt";
	m_detailedLogFile.open(fileName.c_str(), std::ios_base::app);
	m_detailedLogFile << std::setw(10) << m_problemSize << "\n";
	m_detailedLogFile << std::setw(10) << m_image.GetElapsedTime() << "\n";
	m_detailedLogFile << std::setw(10) << m_inputGrid.GetElapsedTime() << "\n";
	m_detailedLogFile << std::setw(10) << m_outputGrid.GetElapsedTime() << "\n";
};

FEAModeller::~FEAModeller()
{
	//Do nothing
}

//void FEAModeller::Resize()
//{
//	auto start = std::chrono::high_resolution_clock::now();
//
//	//for (U64 i = 0; i < m_outputGrid.GetTotalVoxelCount(); i++)
//	//{
//	//	printf("density: %d\n", m_outputGrid.GetVoxelDensity(i));
//	//}
//	if (m_inputVoxelSize == m_outputVoxelSize)
//	{
//		for (U64 i = 0; i < m_inputGrid.GetTotalVoxelCount(); i++)
//		{
//			m_outputGrid.SetVoxelDensity(i, m_inputGrid.GetVoxelDensity(i));
//			//printf("outputGrid.GetVoxelDensity(%d): %d\n", i, inputGrid.GetVoxelDensity(i));
//		}
//		m_outputGrid.SetFilledVoxelCount(m_inputGrid.GetFilledVoxelCount());
//		//printf("filledVoxelCount: %d\n", outputGrid.GetFilledVoxelCount());
//	}
//	else
//	{
//		//Resize Voxels with CUDA
//		ResizeVoxels(m_inputGrid, m_outputGrid);
//	}
//
//	if (m_useOctree)
//	{
//		m_octree.Build(m_outputGrid);
//		printf("The octree has been created.\n");
//	}
//
//	auto elapsed = std::chrono::high_resolution_clock::now() - start;
//	long long elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
//	std::cout << "elapsed time in Resize Voxels: " << elapsedTime << std::endl;
//	m_detailedLogFile << std::setw(10) << elapsedTime << "  ";
//
//}

void FEAModeller::RemoveDuplicateVertices()
{
	//auto start1 = std::chrono::high_resolution_clock::now();

	//std::vector<int> indices;
	//std::vector<Vec3> nodes;
	//indices.assign(m_verticesArray.size(), -1);

	//for (U64 i = 0; i < m_verticesArray.size(); ++i)
	//{
	//	Vec3 vertex = m_verticesArray[i];
	//	if (indices[i] == -1)
	//	{
	//		nodes.push_back(vertex);
	//		for (U64 j = i; j < m_verticesArray.size(); ++j)
	//		{
	//			if (vertex == m_verticesArray[j])
	//			{
	//				indices[j] = nodes.size();
	//			}
	//		}
	//	}
	//}

	//auto elapsed1 = std::chrono::high_resolution_clock::now() - start1;
	//long long elapsedTime1 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed1).count();
	//std::cout << "total length of the array: " << m_verticesArray.size() << std::endl;
	//std::cout << "number of deplicates: " << m_verticesArray.size() - nodes.size() << std::endl;
	//std::cout << "elapsed time for O(n^2): " << elapsedTime1 << std::endl;

	auto start2 = std::chrono::high_resolution_clock::now();


	std::unordered_map<Vec3, size_t> uniqueVertices{};

	for(const auto & vertex : m_verticesArray)
	{
		if (uniqueVertices.count(vertex) == 0)
		{
			uniqueVertices[vertex] = m_nodes.size();
			m_nodesIndices.push_back(m_nodes.size());
			m_nodes.push_back(vertex);
		}
		else
		{
			m_nodesIndices.push_back(uniqueVertices.at(vertex));
		}
	}
	m_elementsCount = m_nodesIndices.size() / 8;
	/*printf("Duplicate nodes are being removed.\n");*/

	auto elapsed2 = std::chrono::high_resolution_clock::now() - start2;
	long long elapsedTime2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed2).count();
	std::cout << "elapsed time Remove Duplicate Nodes: " << elapsedTime2 << std::endl;
	m_detailedLogFile << std::setw(10) << elapsedTime2 << "  ";
}

void FEAModeller::CreateHexas()
{
	auto start = std::chrono::high_resolution_clock::now();

	m_verticesArray.reserve(m_outputGrid.GetFilledVoxelCount() * 8);

	if (m_useOctree)
	{
		//for (const auto & it : m_octree.m_hashTable)
		//{
		//	ivec3 minCornerContainer = it.second.center - it.second.voxelSize / 2;
		//	Vec3 firstVertex = { minCornerContainer.x, minCornerContainer.y, minCornerContainer.z };

		//	Vec3 step = { 0.0f, 0.0f, 0.0f };

		//	//v0
		//	m_verticesArray.push_back(firstVertex + step);

		//	//v1
		//	step.x = it.second.voxelSize.x;
		//	m_verticesArray.push_back(firstVertex + step);

		//	//v2
		//	step.z = -it.second.voxelSize.z;
		//	m_verticesArray.push_back(firstVertex + step);

		//	//v3
		//	step.x = 0.0f;
		//	m_verticesArray.push_back(firstVertex + step);

		//	//v4
		//	step.z = 0.0f;
		//	step.y = it.second.voxelSize.y;
		//	m_verticesArray.push_back(firstVertex + step);

		//	//v5
		//	step.x = it.second.voxelSize.x;
		//	m_verticesArray.push_back(firstVertex + step);

		//	//v6
		//	step.z = -it.second.voxelSize.z;
		//	m_verticesArray.push_back(firstVertex + step);

		//	//v7
		//	step.x = 0.0f;
		//	m_verticesArray.push_back(firstVertex + step);
		//}
	}
	else
	{
		CreateHexahedral(m_outputGrid, m_verticesArray);
	}

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout << "elapsed time in Create Hexahedrals: " << elapsedTime << std::endl;
	m_detailedLogFile << std::setw(10) << elapsedTime << "  ";

	RemoveDuplicateVertices();
}

float FEAModeller::CalculateMaterial(int density)
{
	float result;
	float p = m_slope * static_cast<float>(density) + m_intercept;
	if (p < -22)
	{
		result = 1;
	}
	else if (p > -22 & p < 653)
	{
		result = 34800.0f * pow(p, 2.506f);
	}
	else if (p > 653)
	{
		result = 17691.0f * pow(p, 2.01);
	}
	return result;
}

void FEAModeller::ChangeMaterialResolution(float min, float max, float stepSize)
{
	for (U64 i = 0; i < m_materials.size(); i++)
	{
		int index = (m_materials[i] - min) / stepSize;
		if (m_materials[i] >= 0)
		{
			m_materials[i] = min + (static_cast<float>(index) + 0.5f) * stepSize;
		}
		else
		{
			m_materials[i] = min + (static_cast<float>(index) - 0.5f) * stepSize;
		}
	}
}

void FEAModeller::RemoveDuplicateMaterials()
{
	std::unordered_map<float, size_t> uniqueMaterials{};
	std::vector<int> tempMaterials;
	for (U64 i = 0; i < m_materials.size(); i++)
	{
		int m = m_materials[i];
		if (uniqueMaterials.count(m) == 0)
		{
			uniqueMaterials.insert({ m, tempMaterials.size() });
			m_materialsIndices.push_back(tempMaterials.size());
			tempMaterials.push_back(m);
		}
		else
		{
			m_materialsIndices.push_back(uniqueMaterials.at(m));
		}
	}

	m_materials.clear();
	m_materials.swap(tempMaterials);
}

void FEAModeller::ProcessMaterials()
{
	auto start1 = std::chrono::high_resolution_clock::now();


	float minMaterial = std::numeric_limits<float>::max();
	float maxMaterial = -minMaterial;
	
	if (m_useOctree)
	{
		//for (const auto it : m_octree.m_hashTable)
		//{
		//	int d = it.second.density;
		//	float m = CalculateMaterial(d);
		//	m_materials.push_back(m);
		//	minMaterial = glm::min(minMaterial, m);
		//	maxMaterial = glm::max(maxMaterial, m);
		//}
	}
	else
	{
		for (U64 i = 0; i < m_outputGrid.GetTotalVoxelCount(); i++)
		{
			int intMin = std::numeric_limits<int>::min();
			int d = m_outputGrid.GetVoxelDensity(i);
			if (d != intMin)
			{
				float m = CalculateMaterial(d);
				m_materials.push_back(m);
				minMaterial = glm::min(minMaterial, m);
				maxMaterial = glm::max(maxMaterial, m);
			}
		}
	}
	auto elapsed1 = std::chrono::high_resolution_clock::now() - start1;
	long long elapsedTime1 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed1).count();
	std::cout << "elapsed time in Calculate Material: " << elapsedTime1 << std::endl;
	m_detailedLogFile << std::setw(10) << elapsedTime1 << "  ";


	auto start2 = std::chrono::high_resolution_clock::now();

	if (m_stepSize != 1)
	{
		ChangeMaterialResolution(minMaterial, maxMaterial, m_stepSize);
	}
	auto elapsed2 = std::chrono::high_resolution_clock::now() - start2;
	long long elapsedTime2 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed2).count();
	std::cout << "elapsed time in Change Material Resolution: " << elapsedTime2 << std::endl;
	m_detailedLogFile << std::setw(10) << elapsedTime2 << "  ";

	auto start3 = std::chrono::high_resolution_clock::now();

	for (U64 i = 0; i < m_materials.size(); ++i)
	{
		m_materialsPool.insert({ m_materials[i], i });
	}

	RemoveDuplicateMaterials();


	auto elapsed3 = std::chrono::high_resolution_clock::now() - start3;
	long long elapsedTime3 = std::chrono::duration_cast<std::chrono::microseconds>(elapsed3).count();
	std::cout << "elapsed time in Remove Duplicate Materials: " << elapsedTime3 << std::endl;
	m_detailedLogFile << std::setw(10) << elapsedTime3 << "  ";
	printf("Materials are being processed.\n");


}

void FEAModeller::ExportINP()
{
	auto start = std::chrono::high_resolution_clock::now();

	//initialization
	std::stringstream stream;
	stream << m_outputVoxelSize.x << ", " << m_outputVoxelSize.y << ", " << m_outputVoxelSize.z;
	
	std::string fileNameComplete = m_fileName + "_Complete.inp";
	std::ofstream fileComplete;
	fileComplete.open(fileNameComplete.c_str());


	
	if (m_integrateNodesInfo)
	{
		//Write nodes
		std::ofstream fileNodes;
		std::string fileNameNodes = m_fileName + "_Nodes.inp";
		fileNodes.open(fileNameNodes.c_str());

		fileComplete << "*NODE\n";
		fileComplete.precision(PRECISION + 2);
		fileNodes.precision(PRECISION + 2);
		for (U64 i = 0; i < m_nodes.size(); i++)
		{
			float x = static_cast<float>(m_nodes[i].x);
			float y = static_cast<float>(m_nodes[i].y);
			float z = static_cast<float>(m_nodes[i].z);
			float den = pow(10, PRECISION);
			x /= den;
			y /= den;
			z /= den;
			fileComplete << i + 1 << ", " << x << ", " << y << ", " << z << std::endl;
			fileNodes << i + 1 << ", " << x << ", " << y << ", " << z << std::endl;
		}

		//Write elements
		std::ofstream fileElements;
		std::string fileNameElements = m_fileName + "_Elements.inp";
		fileElements.open(fileNameElements.c_str());

		fileComplete << "*ELEMENT, TYPE=C3D8\n";

		U64 j = 1;
		for (unsigned int i = 0; i < m_nodesIndices.size(); i += 8)
		{
			fileComplete << j << ", ";
			fileElements << j << ", ";

			fileComplete << m_nodesIndices[i] + 1 << ", ";
			fileElements << m_nodesIndices[i] + 1 << ", ";

			fileComplete << m_nodesIndices[i + 1] + 1 << ", ";
			fileElements << m_nodesIndices[i + 1] + 1 << ", ";

			fileComplete << m_nodesIndices[i + 2] + 1 << ", ";
			fileElements << m_nodesIndices[i + 2] + 1 << ", ";

			fileComplete << m_nodesIndices[i + 3] + 1 << ", ";
			fileElements << m_nodesIndices[i + 3] + 1 << ", ";

			fileComplete << m_nodesIndices[i + 4] + 1 << ", ";
			fileElements << m_nodesIndices[i + 4] + 1 << ", ";
			
			fileComplete << m_nodesIndices[i + 5] + 1 << ", ";
			fileElements << m_nodesIndices[i + 5] + 1 << ", ";

			fileComplete << m_nodesIndices[i + 6] + 1 << ", ";
			fileElements << m_nodesIndices[i + 6] + 1 << ", ";

			fileComplete << m_nodesIndices[i + 7] + 1 << "\n";
			fileElements << m_nodesIndices[i + 7] + 1 << "\n";

			++j;
		}
	}
	
	if (m_integrateMaterials)
	{
		//Which materials assigned to which element
		std::ofstream fileElsets;
		std::string fileNameElsets = m_fileName + "_Elsets.inp";
		fileElsets.open(fileNameElsets.c_str());

		std::vector<float> recordMaterial;
		U64 j{ 1 };

		for (std::unordered_multimap<float, int>::iterator iterator = m_materialsPool.begin(), end = m_materialsPool.end(); iterator != end; iterator = m_materialsPool.upper_bound(iterator->first))
		{
			std::pair <std::unordered_multimap<float, int>::iterator, std::unordered_multimap<float, int>::iterator> range = m_materialsPool.equal_range(iterator->first);
			recordMaterial.push_back(iterator->first);
			fileComplete << "*Elset, elset = Set_" << j << "\n";
			fileElsets << "*Elset, elset = Set_" << j << "\n";
			int k{ 1 };
			for (std::unordered_multimap<float, int>::iterator i = range.first; i != range.second; ++i)
			{
					fileComplete << i->second + 1;
					fileElsets << i->second + 1;
					if (k % 10 == 0)
					{
						fileComplete << "\n ";
						fileElsets << "\n ";
					}
					else
					{
						fileComplete << ", ";
						fileElsets << ", ";
					}
					++k;
			}
			fileComplete << "\n" << "*Solid Section, elset = Set_" << j << ", material = Mat_" << j << "\n";
			fileElsets << "\n" << "*Solid Section, elset = Set_" << j << ", material = Mat_" << j << "\n";
			++j;
		}

		//Write materials
		std::ofstream fileMaterials;
		std::string fileNameMaterials = m_fileName + "_Materials.inp";
		fileMaterials.open(fileNameMaterials.c_str());

		for (unsigned int i = 0; i < recordMaterial.size(); i++)
		{
			fileComplete << "*Material, name = Mat_" << i + 1 << "\n";
			fileMaterials << "*Material, name = Mat_" << i + 1 << "\n";
			fileComplete << "*Elastic\n";
			fileMaterials << "*Elastic\n";
			fileComplete << recordMaterial[i] << ", " << MATERIAL_POISSION_RATIO << "\n";
			fileMaterials << recordMaterial[i] << ", " << MATERIAL_POISSION_RATIO << "\n";
		}
	}

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	long long elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout << "elapsed time in Write into File: " << elapsedTime << std::endl;
	m_detailedLogFile << std::setw(10) << elapsedTime << "\n";
	printf("Writing *.INP file is completed.\n");
}

void FEAModeller::ExportTXT()
{
		std::stringstream stream;
		stream << m_outputVoxelSize.x << ", " << m_outputVoxelSize.y << ", " << m_outputVoxelSize.z;
		std::string fileName = "GVMaterials" + stream.str() + "mm_" + m_fileName + ".txt";
		std::ofstream file;
		file.open(fileName.c_str());
		file.precision(PRECISION + 2);
		file.width(30);
		int j{ 0 };

		//ivec3 dim = { 20, 41, 41 };
		//vec3 maxCorner = { 10, 20.5, 20.5 };
		//vec3 minCorner = { -10, -20.5, -20.5 };
		////float voxelSize = 0.03f;
		//vec3 voxelSize = (maxCorner - minCorner) / static_cast<vec3>(dim);
		//printf("voxelSize: %f\n", voxelSize.x);
		//for (int i = 0; i < dim.x; i++)
		//{
		//	for (int j = 0; j < dim.y; j++)
		//	{
		//		for (int k = 0; k < dim.z; k++)
		//		{
		//			vec3 center = minCorner + vec3(i, j, k) * voxelSize + voxelSize / 2.0f;
		//			file << std::setw(10) << center.x << std::setw(10) << center.y << std::setw(10) << center.z << std::setw(10) << 600 << std::endl;
		//		}
		//	}
		//}

		if (m_useOctree)
		{
			//for (const auto & voxel : m_octree.m_hashTable)
			//{
			//	ivec3 voxelCenter = voxel.second.center;
			//	double x = static_cast<double>(voxelCenter.x);
			//	double y = static_cast<double>(voxelCenter.y);
			//	double z = static_cast<double>(voxelCenter.z);
			//	ivec3 voxelSize = voxel.second.voxelSize;
			//	double sx = static_cast<double>(voxelSize.x);
			//	double sy = static_cast<double>(voxelSize.y);
			//	double sz = static_cast<double>(voxelSize.z);
			//	double den = pow(10, PRECISION);
			//	x /= den;
			//	y /= den;
			//	z /= den;
			//	sx /= den;
			//	sy /= den;
			//	sz /= den;
			//	file << x << ", " << y << ", " <<  z << ", ";
			//	file << voxel.second.density << ", ";
			//	file << m_materials[m_materialsIndices[j]];
			//	file << sx << ", " << sy << ", " << sz << ", ";
			//	file << j << "\n";
			//	++j;
			//}
		}
		else
		{
			for (U64 i = 0; i < m_outputGrid.GetTotalVoxelCount(); i++)
			{
				int limit = std::numeric_limits<int>::min();
				if (m_outputGrid.GetVoxelDensity(i) != limit)
				{
					vec3 voxelCenter = m_outputGrid.GetVoxelCenter(i);
					float x = static_cast<float>(voxelCenter.x);
					float y = static_cast<float>(voxelCenter.y);
					float z = static_cast<float>(voxelCenter.z);
					float den = pow(10, PRECISION);
					x /= den;
					y /= den;
					z /= den;
					file << std::setw(8) << x << ", " << std::setw(8) << y << ", " << std::setw(8) << z << ", ";
					file << std::setw(8) << m_outputGrid.GetVoxelDensity(i) << ", ";
					file << std::setw(8) << m_materials[m_materialsIndices[j]] << ", ";
					file << std::setw(12) << j << "\n";
					++j;
				}
			}
		}
	printf("Writing *.TXT file is completed.\n");
}

//void FEAModeller::Render(int render, int writeFile)
//{
//	//Render
//	std::string fileName = m_fileName + ".vtu";
//	VTKInterface interface(m_nodes, m_nodesIndices, { -10000, 10000 }, { 255, 0, 0 }, 0, fileName, render, writeFile);
//	//interface.DoSomething();
//	interface.Render();
//}