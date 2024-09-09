#include "util/melon.h"
#include "util/storage_filesystem.h"

#include "engine.h"

int main(int argc, const char *argv[]) {
	DgError err;
	DgArgs args;
	
	if ((err = DgArgParse(&args, argc, argv))) {
		return 0x01;
	}
	
	gEngine = DgMemoryAllocate(sizeof *gEngine);
	
	if ((err = EngineInit(gEngine, &args))) {
		DgMemoryFree(gEngine);
		return 0x10;
	}
	
	if ((err = EngineRun(gEngine))) {
		DgMemoryFree(gEngine);
		return 0x20;
	}
	
	int ret = EngineFree(gEngine);
	
	DgMemoryFree(gEngine);
	
	return ret;
}
