#pragma once

#include "common.h"
#include "util/table.h"
#include "util/args.h"
#include "rendroar/rendroar.h"

typedef struct Engine {
	DgTable properties;
	
	DgWindow window;
	RoContext roc;
	
	size_t frames;
} Engine;

DgError EngineInit(Engine *this, DgArgs *args);
DgError EngineRun(Engine *this);
int EngineFree(Engine *this);
