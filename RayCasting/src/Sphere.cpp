#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <iostream>

#include "Sphere.h"


using namespace std;
using namespace glm;

bool Sphere::RaySphereIntersection(const RayCasted& rayCasted, HitPoint& hitPoint)
{
	PointLocation pointLocation = isPointinSphere(rayCasted.rayPoint);
	if (pointLocation != PointLocation::OUTSIDE)
	{
		//cout << "Ray is casted from inside (or surface) from sphere!" << endl;

		if (rayCasted.rayType == RayType::PRIMARY)
		{
			//cout << "-- Primary ray! Not doing calculations!" << endl;
			return false;
		}
		else if (rayCasted.rayType == RayType::SHADOW)
		{
			if (dot(rayCasted.shadowPoint->normalRay, rayCasted.rayDir) > -error)
			{
				//cout << "Shadow ray is going out of the sphere, no intersection with the sphere" << endl;
				return false;
			}
			else
			{
				//cout << "Intersection with the sphere" << endl;
				return true;
			}
		}
	}

	float aCof = dot(rayCasted.rayDir, rayCasted.rayDir);

	if (abs(aCof) < error)
	{
		//cout << "No solution: No intersection!" << endl;
		return false;
	}

	vec4 p0MinuspC = rayCasted.rayPoint - vec4(pos, 1.0f);

	float bCof = 2.0f * dot(p0MinuspC, rayCasted.rayDir);
	float cCof = dot(p0MinuspC, p0MinuspC) - (radius * radius);

	float b2_4ac = (bCof * bCof) - (4 * aCof * cCof);

	if (b2_4ac < 0.0f)
	{
		//cout << "b2_4ac is negative! No intersection of ray with this sphere!" << endl;
		return false;
	}
	
	float sq_b2_4ac = pow(b2_4ac, 0.5);

	float t1 = (-bCof + sq_b2_4ac) / (2 * aCof);
	float t2 = (-bCof - sq_b2_4ac) / (2 * aCof);

	vec4 p1 = rayCasted.rayPoint + t1 * rayCasted.rayDir;
	vec4 p2 = rayCasted.rayPoint + t2 * rayCasted.rayDir;

	float d1 = distance2(rayCasted.rayPoint, p1);
	float d2 = distance2(rayCasted.rayPoint, p2);

	if (d1 < d2)
	{
		hitPoint.point = p1;
		hitPoint.normalRay = normalize(p1 - vec4(pos, 1.0f));
	}
	else
	{
		hitPoint.point = p2;
		hitPoint.normalRay = normalize(p2 - vec4(pos, 1.0f));
	}

	//cout << "Intersection point with sphere!" << endl;
	//hitPoint.Print();

	return true;
}

PointLocation Sphere::isPointinSphere(const vec4& iPoint)
{
	float disFromCenter = distance2(iPoint, vec4(pos, 1.0f));
	float r2 = radius * radius;

	if (abs(disFromCenter - r2) < error)
	{
		//cout << "The point is inside or on the sphere!" << endl;
		return PointLocation::SURFACE;
	}
	else if (disFromCenter > r2)
	{
		//cout << "The point is outside of sphere!" << endl;
		return PointLocation::OUTSIDE;
	}

	//cout << "The point is inside the sphere!" << endl;
	return PointLocation::INSIDE;

}