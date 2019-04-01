#include "../include/GrayValues.h"
#include "../include/glmafx.h"

#include "../include/Definitions.h"

#include <cmath>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <limits>
#include <chrono>
#include <iostream>

GrayValues::GrayValues()
	: m_minCorner{}
	, m_maxCorner{}
	, m_dim{}
	, m_voxelCount{}
	, m_centerMid{ 278243, 1085417, 12867 }
	, m_circleCentre{ 315745, 1111505, -39198 }
	, m_anterior{ 270523, 968184, -52833 }
	, m_posterior{ 280728, 1234582, 89663 }
	, m_superior{ 174128, 996526, 189571 }
	, m_inferior{ 415718, 1161892, -138335 }
	, m_ROIMinCorner{ -std::numeric_limits<int>::max(), -std::numeric_limits<int>::max(), -std::numeric_limits<int>::max() }
	, m_ROIMaxCorner{ std::numeric_limits<int>::max(), std::numeric_limits<int>::max(), std::numeric_limits<int>::max() }
	, m_ROICenter{}
{
}

GrayValues::GrayValues(std::string file, vec3 voxelSize) : GrayValues()
{
	auto start = std::chrono::high_resolution_clock::now();

	m_voxelSize = voxelSize * static_cast<float>(pow(10, PRECISION));
	loadFromFile(file, 0);

	auto elapsed = std::chrono::high_resolution_clock::now() - start;
	m_elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
	std::cout << "elapsed time in Read File: " << m_elapsedTime << std::endl;
}

GrayValues::GrayValues(std::string file, vec3 voxelSize, int extractROI) : GrayValues()
{
	m_voxelSize = voxelSize * static_cast<float>(pow(10, PRECISION));
	loadFromFile(file, extractROI);
};

GrayValues::~GrayValues()
{

}

bool GrayValues::loadFromFile(std::string file, int extractROI)
{
	if (extractROI)
	{
		SetROI();
	}

	std::ifstream filestream(file);
	std::string line;
	int intMax = std::numeric_limits<int>::max();
	ivec3 minPoint = { intMax, intMax, intMax };
	ivec3 maxPoint = -minPoint;

	while (std::getline(filestream, line))
	{
		//Read a line
		std::string a, b, c;
		int d;
		std::stringstream lineStream;
		lineStream << line;
		lineStream >> a >> b >> c >> d;
		if (a[0] == '+') { a[0] = '0'; }
		if (b[0] == '+') { b[0] = '0'; }
		if (c[0] == '+') { c[0] = '0'; }

		//Convert to float
		int x = stof(a) * static_cast<float>(pow(10, PRECISION));
		int y = stof(b) * static_cast<float>(pow(10, PRECISION));
		int z = stof(c) * static_cast<float>(pow(10, PRECISION));
		ivec3 point = ivec3(x, y, z);
		mat4 T = { 0.4842694088883013f, -0.5689176459963027f,  0.6646922985029111f, 0.0f,
				  -0.8441210103615656f, -0.5036093033697605f,  0.1839494208347502f, 0.0f,
				   0.2300931539206662f, -0.6501618118848992f, -0.7241179178044725f, 0.0f,
				   0.0f,			     0.0f,                 0.0f,                1.0f };
		point = T * vec4(point, 1.0f);
		if (glm::all(glm::greaterThan(point, m_ROIMinCorner)) && glm::all(glm::lessThan(point, m_ROIMaxCorner)))
		{
			m_centerArray.push_back(point);
			m_densityArray.push_back(d);

			minPoint = glm::min(minPoint, point);
			maxPoint = glm::max(maxPoint, point);
		}
	}
	if (m_centerArray.size() == 0)
	{
		printf("Couldn't read the file\n");
		exit(1);
	}
	m_voxelCount = m_centerArray.size();
	m_minCorner = minPoint - m_voxelSize / 2;
	m_maxCorner = maxPoint + m_voxelSize / 2;
	m_dim = (m_maxCorner - m_minCorner) / m_voxelSize;
	m_dim = glm::max(m_dim, ivec3(1));

	printf("File reading is completed.\n");
	filestream.close();

	return true;
}

void GrayValues::SetROI(void)
{
	ivec3 deltaX = m_posterior - m_anterior;
	ivec3 deltaY = m_superior - m_inferior;

	m_ROIMinCorner = m_centerMid - deltaX - deltaY / 2;
	m_ROIMaxCorner = m_centerMid + deltaX + deltaY / 2;
	m_ROICenter = (m_ROIMaxCorner - m_ROIMinCorner) / 2;
};

void GrayValues::printAllVoxels()
{
	for (unsigned int i = 0; i < m_voxelCount; i++)
	{
		printf("voxel[%d] --> center = (%f, %f, %f), density = %d\n", i, m_centerArray[i].x, m_centerArray[i].y, m_centerArray[i].z, m_densityArray[i]);
	}
}