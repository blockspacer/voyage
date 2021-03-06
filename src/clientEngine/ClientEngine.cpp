#include "ClientEngine.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <signal.h>
#include <common/GameMessages.hpp>
#include "handlers/ClientLoginHandler.hpp"
#include "handlers/ClientCustomHandler.hpp"
#include "handlers/ClientEntitySpawnHandler.hpp"
#include "handlers/ClientWorldStateHandler.hpp"
#include <visibility/VisibilityManager.hpp>
#include <components/BodyComponent.hpp>
#include <components/ClientPlayerSystem.hpp>
#include <components/CollisionComponent.hpp>
#include <components/GraphicsComponent.hpp>
#include <components/ScriptSystem.hpp>
#include <components/ParticleComponent.hpp>
#include <components/RenderSystem.hpp>

//#include "handlers/ClientMapHandler.hpp"
#include "voyage.pb.h"
#include <iostream>

namespace spd = spdlog;
Entity::Id ClientEngine::_player_entity_id;

void ClientEngine::init() {

	std::cout << "Initializing ClientEngine";

	GameEngine::init();

	_netManager.start("127.0.0.1", 1234);
	_netManager.connect();
	_running = true;

	_registerHandlers();

	createWorld();
	//createPlayer();

	// Create client subsystems
	// Subsystems should be created at the end!
	createSubsystems();
}


void ClientEngine::createSubsystems() {

	// TODO: this goes up to the server as well

	// NOTE: Order matters

	// Player system
	std::shared_ptr<ClientPlayerSystem> ps =
		std::make_shared<ClientPlayerSystem>(_world, _netMsgPool, _netManager, _visManager);
	ps->configure(_eventManager);
	add<ClientPlayerSystem>(ps);
	add<ScriptSystem<BodyComponent> >(std::make_shared<ScriptSystem<BodyComponent> >());
	add<CollisionSystem>(std::make_shared<CollisionSystem>(_world));
	add<ParticleSystem>(std::make_shared<ParticleSystem>(_world));


    // Client-only SYSTEMS!

	// updates animations
//	add<AnimationSystem>(std::make_shared<AnimationSystem>());

	// updates graphics objects
	add<GraphicsUpdateSystem>(std::make_shared<GraphicsUpdateSystem>());

	// handles particles
	// std::vector<pumpkin::ParticleRenderer> renderers = {pumpkin::ParticleRenderer(0)};
	// add<ParticleSystem>(std::make_shared<ParticleSystem>(renderers));

	// handles orderly rendering: submits render calls
	// of components so that things display correctly (stencil, depth, etc...)
	add<RenderSystem>(std::make_shared<RenderSystem>(_world, _visManager));
}

void ClientEngine::createWorld() {

	_world = std::make_shared<World>();

    pumpkin::Configuration<pumpkin::Scene::Config> config
            (std::string(CONFIG_FILE_PATH) +
             "scene.cfg");
    std::string fbx_file = config.config().fbx_file();
    FbxLoader loader(std::string(SCENE_FILE_PATH) + fbx_file);

    // Create scene component (for rendering) This should happen only in the client
    Entity scene_entity = _entityManager.createLocal();
    _entityManager.assign<SceneComponent>(scene_entity.id(),
                                          config,
                                          loader);

    // create world
    _world->loadGameLevel(loader);

}

void ClientEngine::createPlayer(uint32_t entity_id, float x, float y) {

	// NO! This id isNetweorked. will be assigned by the server
	//Entity player1 = _entityManager.createNetworked(id);

	Entity player_entity = _entityManager.createFromId(entity_id);
    _player_entity_id = player_entity.id();

	// TODO server spawn msg returns config file and initial params

	pumpkin::Configuration<pumpkin::Animation::Config> config
		(std::string(CONFIG_FILE_PATH) +
		 "main_character_anim3.cfg");
	_entityManager.assign<AnimationComponent>(player_entity.id(),
						  config.config());

	//TODO: REFACTOR THIS code also replicated in server!
	//TODO: This should be a response from a server createEntity message

	_entityManager.assign<PlayerComponent>(player_entity.id(), GameEngine::m_playerId);
	_entityManager.assign<BodyComponent>(player_entity.id(), "cfg");
	_entityManager.assign<NetworkComponent>(player_entity.id());
	_entityManager.assign<CollisionComponent>(player_entity.id());
	_entityManager.assign<InventoryComponent>(player_entity.id());

//	_entityManager.assign<VisibilityComponent>(player_entity.id());

	// Client-side components
	_entityManager.assign<DebugGraphicsComponent>(player_entity.id());


	BodyComponent *body = _entityManager.getComponentPtr<BodyComponent>(player_entity.id());
	body->m_position(0) = x;
	body->m_position(1) = y;


	// create a weapon as well
	// TODO: move this to server
	Entity weapon = _entityManager.createFromId(100);

	_entityManager.assign<WeaponComponent>(weapon.id());
	_entityManager.assign<ItemComponent>(weapon.id());
	_entityManager.assign<BodyComponent>(weapon.id());
	_entityManager.assign<CollisionComponent>(weapon.id());
	_entityManager.assign<NetworkComponent>(weapon.id());

	// add weapon in inventory
	InventoryComponent *inv = _entityManager.getComponentPtr<InventoryComponent>(player_entity.id());
	inv->addItem(weapon.id().id);


// 	Entity player_entity2 = _entityManager.create();
// 	_entityManager.assign<PlayerComponent>(player2.id());
// 	_entityManager.assign<CollisionComponent>(player2.id());
// 	_entityManager.assign<NetworkComponent>(player2.id());
// 	_entityManager.assign<BodyComponent>(player2.id(), "cfg");
// 	_entityManager.assign<AnimationComponent>(player2.id(),
// 						  std::string(CONFIG_FILE_PATH) +
// 						  "main_character_anim3.cfg");
}

void ClientEngine::_registerHandlers() {

	_netManager.registerHandler(std::make_shared<ClientLoginHandler>
				    (ClientEngine::Ptr(this)),
				    {
					    ID_SC_LOGIN_ACCEPTED,
					    ID_CONNECTION_REQUEST_ACCEPTED
				    });

//	_netManager.registerHandler(std::make_shared<ClientCustomHandler<voyage::sc_worldState> >
//				     (&_eventManager), {ID_SC_WORLD_STATE});

	_netManager.registerHandler(std::make_shared<ClientWorldStateHandler>
				    (ClientEngine::Ptr(this)), {ID_SC_WORLD_STATE});

	_netManager.registerHandler(std::make_shared<ClientEntitySpawnHandler>
				    (ClientEngine::Ptr(this)),
				    {
					    ID_SC_SPAWN_PLAYER,
					    ID_SC_SPAWN_ENTITY,
					    ID_SC_DESTROY_ENTITY,
				    });
}

void ClientEngine::requestQuit() {
	_running = false;
}
