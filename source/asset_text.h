#pragma once

#include "assets.h"

typedef struct Text_s {
	union {
		size_t length;
		size_t size;
	};
	void *data;
} Text_s;

typedef const Text_s *Text;

void RegisterTextAssetTypeAndLoader(AssetManager *this);
