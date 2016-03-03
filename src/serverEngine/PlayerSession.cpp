#include "PlayerSession.hpp"
#include "GameServer.hpp"
#include <networking/Messages.hpp>

PlayerSession::PlayerSession(unsigned int id, RakNet::SystemAddress addr, GameServer::Ptr gs) {

	_id = id;
	_gameServer = gs;
	_addr = addr;
	
	
}

void PlayerSession::sendMessage(Message::Ptr m)
{
	if (_gameServer)
		sendRakNetMessage(_gameServer->getRakNetPeer(), m);
}