#include "../include/VTKVoxelRenderer.h"
#include "../include/Definitions.h"

#include <fstream>

VTKInterface::VTKInterface(std::vector<Vec3> nodes, std::vector<U32> indices, vec2 threshold, ivec3 color, int edgeVisbility, std::string fileName, int render, int writeFile)
{

	MakeGrid(nodes, indices, threshold);
	if (writeFile)
	{
		WriteFile(fileName);
	}
	if (render)
	{
		MakeActor(color, edgeVisbility);
	}
}

void VTKInterface::MakeGrid(std::vector<Vec3> nodes, std::vector<U32> indices, vec2 threshold)
{
	auto voxelPoints = vtkSmartPointer<vtkPoints>::New();
	auto cube = vtkSmartPointer<vtkVoxel>::New();
	U32 filledVoxelCount = 0;
	double den = pow(10, PRECISION);
	std::vector<vec3> vertices;
	vertices.assign(8, vec3(0));

	for (U32 j = 0; j < indices.size(); j += 8)
	{
		vertices[0] = { nodes[indices[j + 3]].x / den, nodes[indices[j + 3]].y / den, nodes[indices[j + 3]].z / den };
		vertices[1] = { nodes[indices[j + 2]].x / den, nodes[indices[j + 2]].y / den, nodes[indices[j + 2]].z / den };
		vertices[2] = { nodes[indices[j + 7]].x / den, nodes[indices[j + 7]].y / den, nodes[indices[j + 7]].z / den };
		vertices[3] = { nodes[indices[j + 6]].x / den, nodes[indices[j + 6]].y / den, nodes[indices[j + 6]].z / den };
		vertices[4] = { nodes[indices[j + 0]].x / den, nodes[indices[j + 0]].y / den, nodes[indices[j + 0]].z / den };
		vertices[5] = { nodes[indices[j + 1]].x / den, nodes[indices[j + 1]].y / den, nodes[indices[j + 1]].z / den };
		vertices[6] = { nodes[indices[j + 4]].x / den, nodes[indices[j + 4]].y / den, nodes[indices[j + 4]].z / den };
		vertices[7] = { nodes[indices[j + 5]].x / den, nodes[indices[j + 5]].y / den, nodes[indices[j + 5]].z / den };

		U32 counter = filledVoxelCount;
		for (U32 i = 0; i < 8; ++i)
		{
			cube->GetPointIds()->SetId(i, i + counter * 8);
		}
		m_grid->InsertNextCell(cube->GetCellType(), cube->GetPointIds());
		for (const auto & it : vertices)
		{
			voxelPoints->InsertNextPoint(it.x, it.y, it.z);
		}

		filledVoxelCount++;
	}

	printf("filledVoxelCount: %d\n", filledVoxelCount);

	m_grid->SetPoints(voxelPoints);
}

void VTKInterface::WriteFile(std::string fileName)
{
	auto writer = vtkSmartPointer<vtkXMLUnstructuredGridWriter>::New();
	writer->SetFileName(fileName.c_str());
	writer->SetInputData(m_grid);
	writer->Write();
	printf("Writing the *.VTU file is done.\n");
}

void VTKInterface::MakeActor(ivec3 color, int edgeVisbility)
{
	auto voxelizedMapper = vtkSmartPointer<vtkDataSetMapper>::New();
	voxelizedMapper->SetInputData(m_grid);
	auto voxelizedActor = vtkSmartPointer<vtkActor>::New();
	voxelizedActor->SetMapper(voxelizedMapper);
	voxelizedActor->GetProperty()->SetColor(color.x / 255, color.y / 255, color.z / 255);
	if (edgeVisbility)
	{
		voxelizedActor->GetProperty()->EdgeVisibilityOn();
	}
	m_actors.push_back(voxelizedActor);
}

void VTKInterface::DoSomething()
{
	vtkSmartPointer<vtkPoints> points = m_grid->GetPoints();
	vtkSmartPointer<vtkDataArray> dataArray = points->GetData();
	vtkIdType cellsCount = m_grid->GetNumberOfCells();
	vtkSmartPointer<vtkIdList> cellIndex = vtkSmartPointer<vtkIdList>::New();
	std::vector<float> coords;
	vtkIdType vertexIndex{ 0 };

	for (int i = 0; i < cellsCount; ++i)
	{
			m_grid->GetCellPoints(i, cellIndex);
			vertexIndex = cellIndex->GetId(0);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));

			vertexIndex = cellIndex->GetId(1);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));

			vertexIndex = cellIndex->GetId(2);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));

			vertexIndex = cellIndex->GetId(3);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));

			vertexIndex = cellIndex->GetId(4);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));

			vertexIndex = cellIndex->GetId(5);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));

			vertexIndex = cellIndex->GetId(6);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));

			vertexIndex = cellIndex->GetId(7);
			coords.push_back(dataArray->GetComponent(vertexIndex, 0));
			coords.push_back(dataArray->GetComponent(vertexIndex, 1));
			coords.push_back(dataArray->GetComponent(vertexIndex, 2));
	}


	/*for (vtkIdType i = 0; i < triangles_count; i++) {
		int j = 9 * i;
		mesh->GetCellPoints(i, faceIndex);
		vertexIndex = faceIndex->GetId(0);
		coords[j] = dataArray->GetComponent(vertexIndex, 0);
		coords[j + 1] = dataArray->GetComponent(vertexIndex, 1);
		coords[j + 2] = dataArray->GetComponent(vertexIndex, 2);
		vertexIndex = faceIndex->GetId(1);
		coords[j + 3] = dataArray->GetComponent(vertexIndex, 0);
		coords[j + 4] = dataArray->GetComponent(vertexIndex, 1);
		coords[j + 5] = dataArray->GetComponent(vertexIndex, 2);
		vertexIndex = faceIndex->GetId(2);
		coords[j + 6] = dataArray->GetComponent(vertexIndex, 0);
		coords[j + 7] = dataArray->GetComponent(vertexIndex, 1);
		coords[j + 8] = dataArray->GetComponent(vertexIndex, 2);
	}*/


	printf("hello\n");
}

void VTKInterface::Render()
{
	vtkSmartPointer<vtkAxesActor> axes = vtkSmartPointer<vtkAxesActor>::New();

	//Renderer
	vtkSmartPointer<vtkRenderer> renderer = vtkSmartPointer<vtkRenderer>::New();
	for (U32 i = 0; i < m_actors.size(); i++)
	{
		renderer->AddActor(m_actors[i]);
	}
	
	renderer->AddActor(axes);
	renderer->SetBackground(1, 1, 1);

	//Rendering Window
	vtkSmartPointer<vtkRenderWindow> renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
	renderWindow->AddRenderer(renderer);

	//Interactor
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor = vtkSmartPointer<vtkRenderWindowInteractor>::New();
	renderWindowInteractor->SetRenderWindow(renderWindow);
	vtkSmartPointer<vtkInteractorStyleTrackballCamera> style = vtkSmartPointer<vtkInteractorStyleTrackballCamera>::New();
	renderWindowInteractor->SetInteractorStyle(style);
	renderWindowInteractor->Start();
}

