#pragma once

enum {
	RO_API_GLES = 1,
};

typedef struct {
	int api, major, minor;
} RoContext;
