#include "ServerEngine.hpp"
#include <bx/timer.h>

float getDeltaTime() {
	int64_t now = bx::getHPCounter();
	static int64_t last = now;
	const int64_t frameTime = now - last;
	last = now;
	const double freq = double(bx::getHPFrequency() );
	return float(frameTime/freq);
}

void ServerEngine::init(int port) {

	GameEngine::init();
	GameEngine::isClient = false;
		
	//initialize server
	_netManager.start(port);
	_finished = false;

	if (not _netManager.getPeer()) {
		std::cout << "SERVER raknetpeer null" << std::endl;
		assert(false);
	}

	createWorld();
	createSubsystems();
}

void ServerEngine::processFrame() {
			       
	_netManager.receiveData();
	_systemManager->update_all(getDeltaTime());
}

voyage::sc_worldState ServerEngine::computeWorldState(uint32_t player_entity_id /*world area*/) {

	voyage::sc_worldState ws;
	
	//TODO: send only entities that are "marked" as dirty? (only non players!)
	_entityManager.each<NetworkCmp, BodyCmp>([player_entity_id,
						  this, &ws](Entity entity,
							     NetworkCmp &netdata,
							     BodyCmp & body) {

	     //do not fill the world state with the player entity
	     if (entity.id().id != player_entity_id) {
		     //	     if (netdata.dirty) {
			     voyage::sc_entityState *eu =  ws.add_entity_update(); 
			     eu->set_entityid(entity.id().id);	     
			     eu->set_pos_x(body.m_position(0));
			     eu->set_pos_y(body.m_position(1));
			     eu->set_angle(body.m_rotAngle);
//			     netdata.dirty = false;
//		     }
	          }
	     });


	// how to fill spawn/destroy messages?
	

	return ws;
}


void ServerEngine::createWorld() {
	_world = std::make_shared<World>();
}


Entity ServerEngine::createPlayerEntity(int player_id) {

	Entity entity = _entityManager.createNetworked();

	_entityManager.assign<PlayerCmp>(entity.id(), player_id);
	_entityManager.assign<BodyCmp>(entity.id(), "cfg");	
	_entityManager.assign<NetworkCmp>(entity.id());		
	_entityManager.assign<CollisionComponent>(entity.id());


	// initial position of player?
	BodyCmp *body = _entityManager.getComponentPtr<BodyCmp>(entity.id());
	body->m_position(0) = 0.8;
	body->m_position(1) = 0.8;
								
	
	return entity;
}

void ServerEngine::createSubsystems() {
	
	// Create playerSystem with no network manager
	std::shared_ptr<ServerPlayerSystem> ps =
		std::make_shared<ServerPlayerSystem>(_world, _netManager);
	ps->configure(_eventManager);
	add<ServerPlayerSystem>(ps);
	
}
