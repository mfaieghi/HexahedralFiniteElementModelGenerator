cmake_minimum_required(VERSION 2.8)

project(FEAModeller-C)

set(SOURCE_FILES source/main.cpp source/Grayvalues.cpp source/FEAModeller.cpp source/Octree.cpp source/CreateHexahedral.cpp source/InpWriter.cpp source/ResizeVoxels.cpp source/VoxelGrid.cpp)

set(HEADER_FILES include/glmafx.h include/Definitions.h include/GrayValues.h include/HashTable.h include/PrimitiveTypes.h include/Vec3.h include/FEAModeller.h include/Octree.h include/CreateHexahedral.h include/VoxelGrid.h include/ResizeVoxels.h include/InpWriter.h)

include_directories(external)

add_executable(FEAModeller-C ${SOURCE_FILES} ${HEADER_FILES})

