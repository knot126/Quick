#include "util/melon.h"
#include "util/storage_filesystem.h"

#include "engine.h"

int main(int argc, const char *argv[]) {
	DgError err;
	DgArgs args;
	
	if ((err = DgArgParse(&args, argc, argv))) {
		return 0x01;
	}
	
	Engine *engine = DgMemoryAllocate(sizeof *engine);
	
	if ((err = EngineInit(engine, &args))) {
		DgMemoryFree(engine);
		return 0x10;
	}
	
	if ((err = EngineRun(engine))) {
		DgMemoryFree(engine);
		return 0x20;
	}
	
	int ret = EngineFree(engine);
	
	DgMemoryFree(engine);
	
	return ret;
}
