#pragma once
#include <glm/glm.hpp>
#include <stdio.h>

using namespace std;
using namespace glm;

enum class RayType
{
	PRIMARY = 0, SHADOW
};

struct HitPoint
{
	vec4 point;
	vec4 normalRay;

	HitPoint(vec4 p = vec4(vec3(0.0f),1.0f), vec4 n = vec4(0.0f))
		:point(p), normalRay(n)
	{

	}

	void Print()
	{

		printf("\tpoint = (%f, %f, %f, %f) \n", point.x, point.y, point.z, point.w);
		printf("\tnormalRay = (%f, %f, %f, %f) \n", normalRay.x, normalRay.y, normalRay.z, normalRay.w);

	}
};

struct RayCasted
{
	vec4 rayPoint;
	vec4 rayDir;
	RayType rayType;
	HitPoint* shadowPoint;

	RayCasted(vec4 rP = vec4(vec3(0.0f),1.0f), vec4 rD = vec4(0.0f), RayType rT = RayType::PRIMARY, HitPoint* sP = nullptr)
		:rayPoint(rP), rayDir(rD), rayType(rT), shadowPoint(sP)
	{

	}
};

