#include "ScriptEngine.hpp"
#include <database/DatabaseConnectionPool.hpp>
#include <spdlog/spdlog.h>
#include <gtest/gtest.h>
#include <serverEngine/PlayerSession.hpp>

namespace spd = spdlog;
auto console = spd::stdout_logger_mt("Server");

TEST(scriptTest, callScript) {

	DatabaseConnectionPool::Ptr pool = std::make_shared<DatabaseConnectionPool>(10);
	pool->fillPool(DB_DIR "/test.db");

	ScriptEngine::Ptr se = std::make_shared<ScriptEngine>();
	se->init(pool);
	
	//PlayerSession::Ptr ps = std::make_shared<PlayerSession>();
	//se->runScript(ps, SCRIPTS_DIR "/test.py");
}
