
#include "core/Engine.h"
#include "StateEntry.h"

#include "io/File.h"
#include "state/StateManager.h"

#include <plog/Log.h>

int main(int argc, char *argv[])
{
	util::File log{ "log.log" };
	log.erase();
	plog::init(plog::debug, log.path().c_str());

	core::Engine engine;
	engine.initialize();
	engine.getStates().set<game::StateEntry>();
	engine.start();
	return 0;
}
