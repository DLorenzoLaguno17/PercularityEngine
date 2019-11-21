#include "Intersection.h"

bool Intersect(Frustum frustum, AABB aabb)
{
	Plane planes[6];
	float3 corners[8];

	frustum.GetPlanes(planes);
	aabb.GetCornerPoints(corners);

	for (int p = 0 ; p < 6 ; ++p )
	{
		int inCount = 8;

		for (int c = 0; c < 8; ++c)
		{
			if (planes[p].IsOnPositiveSide(corners[c]))
				inCount--;
		}
		if (inCount == 0)
			return false;
	}
	return true;

}

