#pragma once

#include "../include/glmafx.h"

struct Vec3
{
	int x;
	int y;
	int z;

	bool operator == (const Vec3 &other) const
	{
		//int temp = x * 1000;
		//float lX = static_cast<float>(temp) / 1000.0f;
		//temp = y * 1000;
		//float lY = static_cast<float>(temp) / 1000.0f;
		//temp = z * 1000;
		//float lZ = static_cast<float>(temp) / 1000.0f;

		//temp = other.x * 1000;
		//float rX = static_cast<float>(temp) / 1000.0f;
		//temp = other.y * 1000;
		//float rY = static_cast<float>(temp) / 1000.0f;
		//temp = other.z * 1000;
		//float rZ = static_cast<float>(temp) / 1000.0f;

		//return (lX == rX) && (lY == rY) && (lZ == rZ);

		//int lX = static_cast<int>(x * 1000.0f);
		//int lY = static_cast<int>(y * 1000.0f);
		//int lZ = static_cast<int>(z * 1000.0f);
		//int rX = static_cast<int>(other.x * 1000.0f);
		//int rY = static_cast<int>(other.y * 1000.0f);
		//int rZ = static_cast<int>(other.z * 1000.0f);
		//return (lX == rX) && (lY == rY) && (lZ == rZ);

		return (x == other.x) && (y == other.y) && (z == other.z);
	}

	Vec3 operator () (const vec3 &other)
	{
		Vec3 result = { other.x, other.y, other.z };
		return result;
	}


	Vec3 operator + (const Vec3 & other) const
	{
		Vec3 result = { x + other.x, y + other.y, z + other.z };
		return result;
	}

	Vec3 operator - (const Vec3 & other) const
	{
		Vec3 result = { x - other.x, y - other.y, z - other.z };
		return result;
	}

	Vec3 operator / (int other) const
	{
		Vec3 result = { x / other, y / other, z / other };
		return result;
	}
};