#pragma once

#include <networking/Session.hpp>
#include <sql/DatabaseConnectionPool.hpp>

class GameServer;
typedef std::shared_ptr<GameServer> GameServerPtr;

class PlayerSession : public Session {

public:

	typedef std::shared_ptr<PlayerSession> Ptr;

	PlayerSession(unsigned int id, RakNet::SystemAddress addr, GameServerPtr _gs);
	
	void sendMessage(Message::Ptr m);
	
private:

	unsigned int _id;
	RakNet::SystemAddress _addr;
	
	GameServerPtr _gameServer;
	DatabaseConnectionPool::Ptr _dbPool;
};
