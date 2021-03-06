#pragma once

#include <Eigen/Dense>
#include <utils/GeometryUtils.hpp>
#include <visibility/Visibility.hpp>
#include <memory>
#include <iostream>

struct LevelRoom {

	LevelRoom() {}

	LevelRoom(int id,
		  std::vector<FbxLoader::FPolygon> rps,
		  std::vector<FbxLoader::FPolygon> cps) : node_id(id) {

		for (auto rp : rps) {
			room_polygons.push_back(GeometryUtils::convertToPolygon(rp));
		}

		for (auto cp : cps)  {
			collision_polygons.push_back(GeometryUtils::convertToPolygon(cp));
		}
	}

	int node_id;
	std::vector<GeometryUtils::Polygon> room_polygons;
	std::vector<GeometryUtils::Polygon> collision_polygons;
};

class GameLevel {

public:

	bool getRoomFromPos(const Vec3f & pos, LevelRoom & roompos) const {

		// search which polygon pos falls in
		for (auto room : m_rooms) {
			if (GeometryUtils::isWithin(pos, room.room_polygons)) {
				roompos = room;
				return true;
			}
		}

		return false;
	}

	std::vector<LevelRoom> getAdjacentRooms(const LevelRoom & levelRoom) const {
		std::vector<LevelRoom> adj_rooms;
		adj_rooms.push_back(levelRoom);
		return adj_rooms;
	}

	std::vector<visibility::LineSegment> getCollisionLineSegments() {

		std::vector<visibility::LineSegment> segments;

		// we could speed up visibility if we limit the polygons to nearby rooms
		// and of course simplifying the geometry is the most important
		for (auto r : m_rooms) {
			for (auto pol : r.collision_polygons) {
				for (auto line : pol.toLineSegmentList())
					segments.push_back(line);
			}
		}
		return segments;
	}

	std::vector<GeometryUtils::Polygon> getCollisionPolygonsNearby(const Vec3f & pos) {

		// check in which room the player is
		LevelRoom roomInPos;
		getRoomFromPos(pos, roomInPos);

		std::vector<LevelRoom> rooms = getAdjacentRooms(roomInPos);

		std::vector<GeometryUtils::Polygon> collision_polygons;
		for (auto r : rooms) {
			for (auto pol : r.collision_polygons) {
				collision_polygons.push_back(pol);
			}
		}

		return collision_polygons;
	}

	std::vector<LevelRoom>  getRoomVector() {return m_rooms; }

	typedef std::shared_ptr<GameLevel> Ptr;

	void load(const FbxLoader & loader);

private:

	std::vector<LevelRoom> m_rooms;

	//Camera

	//Lights

	//?

};
