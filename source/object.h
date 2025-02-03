/**
 * Objects in the engine. This might include Scenes and Entities
 */

#include "common.h"

typedef const char *TrSelector;

struct TrObject;

typedef struct TrObject {
	size_t refs;
	struct TrObject *type;
	DgTable slots;
} TrObject;

typedef TrObject *(*TrMethodImp)(TrObject *this, DgArray *args);

typedef struct TrObjectType {
	TrObject parent;
	const char *name;
	size_t size;
	DgTable methods;
} TrObjectType;
