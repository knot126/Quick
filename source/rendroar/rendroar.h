#pragma once

#include "util/maths.h"
#include "glad/egl.h"
#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct {
	Display *display;
	EGLDisplay egl_display;
	EGLConfig egl_config;
	EGLSurface egl_surface;
	EGLContext egl_context;
	DgVec2I size;
} RoContext;

DgError RoContextCreate(RoContext * const context, DgVec2I size);
void RoContextDestroy(RoContext * const context);
