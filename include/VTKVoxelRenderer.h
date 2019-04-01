#pragma once

#include "../include/Vec3.h"
#include "../include/PrimitiveTypes.h"

#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkInteractorStyleTrackball.h>
#include <vtkSmartPointer.h>
#include <vtkAxesActor.h>
#include <vtkUnstructuredGrid.h>
#include <vtkVoxel.h>
#include <vtkDataSetMapper.h>
#include <vtkXMLUnstructuredGridWriter.h>


#include <vector>

class VTKInterface
{
public:
	VTKInterface::VTKInterface(std::vector<Vec3> nodes, std::vector<U32> indices, vec2 threshold, ivec3 color, int edgeVisbility, std::string fileName, int render, int writeFile);
	void MakeGrid(std::vector<Vec3> nodes, std::vector<U32> nodesIndices, vec2 threshold);
	void WriteFile(std::string fileName);
	void MakeActor(ivec3 color, int edgeVisbility);
	void Render(void);
	void DoSomething();

private:
	vtkSmartPointer<vtkUnstructuredGrid> m_grid = vtkSmartPointer<vtkUnstructuredGrid>::New();
	std::vector<vtkSmartPointer<vtkActor>> m_actors;
};
