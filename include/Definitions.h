#pragma once

//The name of the input gray value file wihtout the extension
#define INPUT_FILE_NAME "QCT-16-05021R"


//Must be at least one more than the decimal points of the voxel size
#define PRECISION 4

//The size CT voxels
#define VOXEL_SIZE_X 0.668f
#define VOXEL_SIZE_Y 0.668f
#define VOXEL_SIZE_Z 0.625f


//The size of voxels in the desired FEA model
#define DESIRED_VOXEL_SIZE_X 0.668f
#define DESIRED_VOXEL_SIZE_Y 0.668f
#define DESIRED_VOXEL_SIZE_Z 0.625f


//Must set to one if material/node info is desired in the FEA model
#define INTEGRATE_NODE_INFO 1
#define INTEGRATE_MATERIAL 1



//material = MATERIAL_SLOP * density + MATERIAL_INTERCEPT
//The values must end by the letter f at the end. For example: 1.0f or 1.2f 
#define MATERIAL_SLOPE 0.015251f
#define MATERIAL_INTERCEPT 0.00067f
#define MATERIAL_POISSION_RATIO 0.3f


//The width of material buckets in order to reduce the resolution of materials
#define MATERIAL_STEP_SIZE 1

//Must be set to 1 if you multi-resolution voxel grid is desired
#define OCTREE 0

//Define the largest voxel size desirable in the voxel grid
// largestVoxelSize = inputVoxelSize * 2 ^ VOXEL_GROWTH_POWER 
#define VOXEL_GROWTH_POWER 5