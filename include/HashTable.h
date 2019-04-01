#pragma once

#include "../include/Vec3.h"

#include <unordered_map>


namespace std
{
	template<>
	struct hash<Vec3>
	{
		void hash_combine(size_t &seed, const size_t &hash) const
		{
			seed ^= hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}

		size_t operator() (const Vec3 &vertex) const
		{
			auto hasher = hash<int>{};
			auto hashed_x = hasher(vertex.x);
			auto hashed_y = hasher(vertex.y);
			auto hashed_z = hasher(vertex.z);

			size_t seed = 0;
			hash_combine(seed, hashed_x);
			hash_combine(seed, hashed_y);
			hash_combine(seed, hashed_z);

			return seed;

		}
	};
}