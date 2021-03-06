#pragma once

#include <game/World.hpp>
#include <utils/GeometryUtils.hpp>

namespace CollisionManager {

	bool isColliding(World::Ptr world, const Vec3f & body_pos, const Vec3f & move_vec);
	Vec3f computeCorrectionVector(World::Ptr world,
				      const Vec3f & body_pos,
				      GeometryUtils::Polygon player,
				      const Vec3f & move_vec);
};
