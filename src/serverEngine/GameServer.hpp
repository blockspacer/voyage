#pragma once

#include <RakPeerInterface.h>
#include <networking/ServerNetworkManager.hpp>
#include <scripts/ScriptEngine.hpp>
#include <database/DatabaseConnectionPool.hpp>
#include <game/World.hpp>

class PlayerSession;
typedef std::shared_ptr<PlayerSession> PlayerSessionPtr;

class GameServer {
public:
	GameServer(int port, int poolSize);

	void start();
	void stop();
	PlayerSessionPtr createPlayerSession(RakNet::SystemAddress);
	

	PlayerSessionPtr getPlayerSession(RakNet::SystemAddress addr) {
		return _playerSessionAddrMap[addr];
	}
	
	RakNet::RakPeerInterface *getRakNetPeer() { _netManagerPtr->getPeer(); }
	ScriptEngine::Ptr getScriptEngine() { return _scriptEnginePtr; }
	World::Ptr getWorld() {return _world;}
	
	typedef std::shared_ptr<GameServer> Ptr;
	
	
private:
	void _init(int port);
	
	DatabaseConnectionPool::Ptr _dbPoolPtr;
	ScriptEngine::Ptr _scriptEnginePtr;
	ServerNetworkManager::Ptr _netManagerPtr;

	std::vector<PlayerSessionPtr> _playerSessions;
	std::map<RakNet::SystemAddress, PlayerSessionPtr> _playerSessionAddrMap; 

	//TODO: several worlds?
	World::Ptr _world;
	
	bool _finished;
	std::string _datasetName;
};

