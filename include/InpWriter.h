#pragma once

#include "../include/Vec3.h"
#include "../include/PrimitiveTypes.h"

#include <string>
#include <vector>
#include <iostream>

extern void exportINP(std::string name, vec3 outputVoxelSize, U64 filledVoxelCount, Vec3 * verticesArray, int includeMaterial);
extern void includeMaterial(int * materialsArray, int materialsIndexArray, U64 numberOfMaterials);
