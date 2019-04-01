#include "../include/VoxelGrid.h"


VoxelGrid::VoxelGrid()
{

}

VoxelGrid::VoxelGrid(GrayValues image, vec3 voxelSize)
	: m_voxelSize { static_cast<ivec3>(pow(10.0f, PRECISION) * voxelSize)}
{
	ivec3 imageMinCorner = image.GetMinCorner() - m_voxelSize;
	ivec3 imageMaxCorner = image.GetMaxCorner() + m_voxelSize;
	m_dim = (imageMaxCorner - imageMinCorner) / m_voxelSize;
	//m_dim = image.GetDim();
	m_dim = glm::max(m_dim, ivec3(1));
	m_totalVoxelCount = m_dim.x * m_dim.y * m_dim.z;
	m_minCorner = imageMinCorner;
	m_maxCorner = imageMaxCorner;
	m_firstVoxelCenter = m_minCorner + m_voxelSize / 2;
	m_densityArray = new unsigned int[m_totalVoxelCount];
	memset(m_densityArray, 0, sizeof(unsigned int) * m_totalVoxelCount);
	m_filledVoxelCount = 0;
	for (U64 i = 0; i < image.GetVoxelCount(); i++)
	{
		ivec3 coords = GetGridCoordsFromPoint(image.GetVoxelCenter(i));
		U64 index = GetIndexFromGridCoords(coords);
		m_densityArray[index] = image.GetVoxelDensity(i);
		m_filledVoxelCount++;
	}
}

//VoxelGrid::VoxelGrid(vec3 modelMinCorner, vec3 modelMaxCorner, vec3 voxelSize)
//	: m_voxelSize{ voxelSize }
//	, m_minCorner { modelMinCorner }
//	, m_filledVoxelCount { }
//{
//	m_dim = static_cast<ivec3>(glm::ceil(((modelMaxCorner + 0.0001f - modelMinCorner) / m_voxelSize)));
//	m_dim = glm::max(m_dim, ivec3(1));
//	m_maxCorner = m_minCorner + static_cast<vec3>(m_dim) * m_voxelSize;
//	m_firstVoxelCenter = m_minCorner + m_voxelSize / 2.0f;
//	m_totalVoxelCount = m_dim.x * m_dim.y * m_dim.z;
//	m_densityArray = new unsigned int[m_totalVoxelCount];
//	memset(m_densityArray, 0, sizeof(unsigned int) * m_totalVoxelCount);
//}

VoxelGrid::VoxelGrid(VoxelGrid in, vec3 desiredVoxelSize, int shouldSmooth)
{
	if (in.GetVoxelSize() == static_cast<ivec3>(pow(10.0f, PRECISION) * desiredVoxelSize))
	{
		m_voxelSize = in.GetVoxelSize();
		m_minCorner = in.GetMinCorner();
		m_dim = in.GetDim();
		m_totalVoxelCount = in.GetTotalVoxelCount();
		m_filledVoxelCount = in.GetFilledVoxelCount();
		m_maxCorner = in.GetMaxCorner();
		m_firstVoxelCenter = in.GetFirstVoxelCenter();
		m_densityArray = new unsigned int[m_totalVoxelCount];
		memcpy(m_densityArray, in.GetDensityArray(), m_totalVoxelCount);
	}
	else
	{
		auto start = std::chrono::high_resolution_clock::now();

		if (glm::mod(static_cast<vec3>(static_cast<int>(pow(10.0f, PRECISION)) * in.GetVoxelSize()), pow(10.0f, PRECISION) * desiredVoxelSize) != vec3(0.0f))
		{
			printf("Desired voxel size must be a divisor of the native CT voxels.\n");
			exit(1);
		}
		else
		{
			m_voxelSize = static_cast<ivec3>(pow(10.0f, PRECISION) * desiredVoxelSize);
			m_minCorner = in.GetMinCorner();
			//m_dim = static_cast<ivec3>(glm::ceil(((in.GetMaxCorner() - in.GetMinCorner()) / m_voxelSize)));
			ivec3 n = in.GetVoxelSize() / m_voxelSize;
			m_dim = n * in.GetDim();
			//m_dim = glm::max(m_dim, ivec3(1));
			m_totalVoxelCount = m_dim.x * m_dim.y * m_dim.z;
			m_maxCorner = m_minCorner + m_dim * m_voxelSize;
			m_firstVoxelCenter = m_minCorner + m_voxelSize / 2;
			m_densityArray = new unsigned int[m_totalVoxelCount];

			memset(m_densityArray, 0, sizeof(unsigned int) * m_totalVoxelCount);

			U64 counter = 0;
			U64 inTotalVoxelCount = in.GetTotalVoxelCount();
			ivec3 inDim = in.GetDim();
			ivec3 inMinCorner = in.GetMinCorner();
			ivec3 inVoxelSize = in.GetVoxelSize();
			for (U64 i = 0; i < inTotalVoxelCount; ++i)
			{
				if (in.GetVoxelDensity(i) != 0)
				{
					U64 z = i / (inDim.x * inDim.y);
					U64 residue = i - z * inDim.x * inDim.y;
					U64 y = residue / inDim.x;
					U64 x = residue - y * inDim.x;

					//ivec3 inVoxelFirstVertex = inMinCorner + ivec3(x, y, z) * inVoxelSize;

					ivec3 N = inVoxelSize / m_voxelSize;
					ivec3 outMinLocalID = ivec3(x, y, z) * N;
					ivec3 outMaxLocalID = ivec3(x + 1, y + 1, z + 1) * N;

					for (U64 j = outMinLocalID.z; j < outMaxLocalID.z; j++)
					{
						for (U64 k = outMinLocalID.y; k < outMaxLocalID.y; k++)
						{
							for (U64 l = outMinLocalID.x; l < outMaxLocalID.x; l++)
							{
								U64 outVoxelIndex = static_cast<U64>(l) + m_dim.x * static_cast<U64>(k) + m_dim.x * m_dim.y * static_cast<U64>(j);
								m_densityArray[outVoxelIndex] = in.GetVoxelDensity(i);
								counter++;
							}
						}
					}
				}
			}
			m_filledVoxelCount = counter;

			if (shouldSmooth)
			{
				for (U64 i = 1; i < m_dim.x - 1; ++i)
				{
					for (U64 j = 1; j < m_dim.y - 1; ++j)
					{
						for (U64 k = 1; k < m_dim.z - 1; ++k)
						{
							U64 sum = 0;
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j - 1, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j - 1, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j - 1, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j + 1, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j + 1, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i - 1, j + 1, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j - 1, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j - 1, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j - 1, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j + 1, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j + 1, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i, j + 1, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j - 1, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j - 1, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j - 1, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j, k + 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j + 1, k - 1))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j + 1, k))]);
							sum += static_cast<U64>(m_densityArray[GetIndexFromGridCoords(ivec3(i + 1, j + 1, k + 1))]);

							m_densityArray[GetIndexFromGridCoords(ivec3(i, j, k))] = static_cast<unsigned int>(sum / 27);
						}
					}
				}
			}
		}

		auto elapsed = std::chrono::high_resolution_clock::now() - start;
		m_elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(elapsed).count();
		std::cout << "elapsed time in Resizing Voxels: " << m_elapsedTime << std::endl;

	}


}



VoxelGrid::~VoxelGrid()
{
	//do nothing for now
}

ivec3 VoxelGrid::GetGridCoordsFromPoint(ivec3 point)
{
	return (point - m_minCorner) / m_voxelSize;
}

//U64 VoxelGrid::GetIndexFromGridCoords(ivec3 gridCoords)
//{
//	return { static_cast<U64>(gridCoords.x) + m_dim.x * static_cast<U64>(gridCoords.y) + m_dim.x * m_dim.y * static_cast<U64>(gridCoords.z) };
//}
//
//ivec3 VoxelGrid::GetGridCoordsFromIndex(U64 index)
//{
//	int z = index / (m_dim.x * m_dim.y);
//	int y = (index - z * m_dim.x * m_dim.y) / m_dim.x;
//	int x = index - z * m_dim.x * m_dim.y - y * m_dim.x;
//	return { x, y, z };
//}

U64 VoxelGrid::GetIndexFromGridCoords(ivec3 gridCoords)
{
	return{ static_cast<U64>(gridCoords.x) + m_dim.x * static_cast<U64>(gridCoords.y) + m_dim.x * m_dim.y * static_cast<U64>(gridCoords.z) };
}

ivec3 VoxelGrid::GetGridCoordsFromIndex(U64 index)
{
	int z = index / (m_dim.x * m_dim.y);
	int temp = index - z * m_dim.x * m_dim.y;
	int y = temp / m_dim.x;
	int x = temp - y * m_dim.x;
	return{ x, y, z };
}

ivec3 VoxelGrid::GetVoxelFristVertexFromIndex(U64 index)
{
	ivec3 inGridCoords = GetGridCoordsFromIndex(index);
	ivec3 coords = m_minCorner + inGridCoords * m_voxelSize;
	return coords;
}

ivec3 VoxelGrid::GetVoxelCenter(U64 index)
{
	ivec3 inGridCoords = GetGridCoordsFromIndex(index);
	vec3 coords = inGridCoords * m_voxelSize + m_voxelSize / 2;
	return coords;
}
