#pragma once

#include "util/maths.h"
#include "glad/egl.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
	float x, y, z;
	float u, v;
	uint8_t r, g, b, a;
} RoVertex;

typedef struct {
	// Multi-frame state
	Display *display;
	EGLDisplay egl_display;
	EGLConfig egl_config;
	EGLSurface egl_surface;
	EGLContext egl_context;
	DgVec2I size;
	
	DgColour background;
	
	// Single frame state
	DgMemoryStream *verticies;
	size_t vertex_count;
	DgMemoryStream *indexes;
	size_t index_count;
} RoContext;

DgError RoContextCreate(RoContext * const context, DgVec2I size);
void RoContextDestroy(RoContext * const context);

DgError RoDrawBegin(RoContext * const this);
DgError RoDrawEnd(RoContext * const this);
DgError RoGetFrameData(RoContext * const this, size_t size, void *data, bool alpha);
