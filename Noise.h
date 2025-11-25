#pragma once

#include<cstdint>
#include<DirectXMath.h>
#include<EASTL/vector.h>


struct Vec2_t
{
	float x = 0;
	float y = 0;
};

class Perlin
{
public:

	void SetGridAndScale(const uint32_t gridX, const uint32_t gridY, const float scale);
	float* getPerlineNose(void) const;
	
	void release(void);

private:

	void generatePerlinNoise(void);
	float calPerlin(const float& x, const float& y);

private:

	const float tarX = 0.3;
	const float tarY = 0.6;

	uint32_t gridX = 0;
	uint32_t gridY = 0;
	float scale = 0;

	Vec2_t* vectorList = nullptr;
	float* perlinNoise = nullptr;
};