#include "GameClient.hpp"
#include <spdlog/spdlog.h>
#include <thread>
#include <signal.h>
#include <graphics/SDL/SDLGraphicsEngine.hpp>
#include <common/GameMessages.hpp>
#include "handlers/ClientLoginHandler.hpp"
#include "handlers/ClientMapHandler.hpp"
#include "voyage.pb.h"

namespace spd = spdlog;

bool gRunning = true;

void exit(int s) {
	gRunning = false;
}

void GameClient::init() {

	_netManager = std::make_shared<ClientNetworkManager>();
	_netManager->start("127.0.0.1", 1234);
	_netManager->connect();
	_running = true;

	_tileMap = std::make_shared<ClientTileMap>();
	
	_registerHandlers();
	
	_graphicsEngine = std::make_shared<SDLGraphicsEngine>(GameClient::Ptr(this));
	_graphicsEngine->init();

	
	
}

void GameClient::_registerHandlers() {

	_netManager->registerHandler(std::make_shared<ClientLoginHandler>(GameClient::Ptr(this)),
				     {ID_SC_LOGIN_ACCEPTED});

	_netManager->registerHandler(std::make_shared<ClientMapHandler>(GameClient::Ptr(this)),
				     {ID_SC_REGION_DATA});
}

void GameClient::clientLoop() {
	
	while (_running && gRunning) {
		
		//_netManager.sendData(ID_DATA_MESSAGE, cmd.c_str());
	}
}

void GameClient::requestQuit() {
	_running = false;
}

void GameClient::initPlayer(PlayerData & playerData) {

}

//main loop
void GameClient::start() {

	auto console = spd::stdout_logger_mt("console");
	console->info("Starting game client");

	//TODO: if I don't sleep the message is not sent!!!
	sleep(1);

	signal(SIGINT, exit);

	voyage::cs_loginRequest msg;
	msg.set_username("jose");
	msg.set_password("1234");
	
	_netManager->sendData<voyage::cs_loginRequest>(ID_CS_LOGIN_REQUEST, msg);
	
	while (_running && gRunning) {

		//check for user input
		
		//send messages to
		
		//receive messages from server
		_netManager->receiveData();

		//update world
		
		
		//display graphics
		_graphicsEngine->run();
		
		//play sounds
	}

	
}
