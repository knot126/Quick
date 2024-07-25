#pragma once

#include "util/array.h"
#include "util/table.h"

typedef struct {
	/**
	 * A context contains all of the loaded objects. It should also probably contain
	 * some kind of resource manager for *.script resources, honestly.
	 */
	
	DgArray objects;
} TrContext;

typedef struct {
	/**
	 * 
	 */
	
	DgArray objects;
} TrCollection;
