#pragma once

#include <common/math.hpp>

namespace GeometryUtils {	
	
	struct AABB {

		AABB() {};
		AABB(Vec3f min, Vec3f max) : m_min(min), m_max(max) {}

		Vec3f m_min;
		Vec3f m_max;
	};
	
	struct Polygon {
		Vec2f vertex[4];
		Vec2f centroid() { return (vertex[0] + vertex[1] + vertex[2] + vertex[3])/4.0f; }
		int num_vertices = 4;
	};

	bool IntersectMTD(GeometryUtils::Polygon A, GeometryUtils::Polygon B, Vec2f& MTD);
			
	bool linePlaneIntersection(Vec3f ray, Vec3f rayOrigin, Vec3f normal,
				   Vec3f point_in_plane, Vec3f & contact);
	
}
