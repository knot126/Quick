#include "util/melon.h"
#define GLAD_EGL_IMPLEMENTATION
#include "glad/egl.h"
#undef GLAD_EGL_IMPLEMENTATION
#include "glad/egl.h"
#define GLAD_GLES2_IMPLEMENTATION
#include "glad/gles2.h"
#include "rendroar.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

DgError RoContextCreate(RoContext * const this, DgVec2I size) {
	/**
	 * Create a new context
	 * 
	 * @warning You can only have one context at a time atm.
	 * 
	 * @param this context
	 * @return error while initialising context
	 */
	
	this->size = size;
	
	// Get the default X display
	Display *display = XOpenDisplay(NULL);
	
	if (!display) {
		DgLog(DG_LOG_ERROR, "Failed to open X display.");
		return DG_ERROR_FAILED;
	}
	
	this->display = display;
	
	// Note that we don't need to create any window.
	
	// Load EGL wihtout display so we can get eglGetDisplay
	int egl_version = gladLoaderLoadEGL(NULL);
	
	if (!egl_version) {
		DgLog(DG_LOG_ERROR, "Initial loading of EGL failed.");
		return DG_ERROR_FAILED;
	}
	
	// Get the EGL abstract display
	EGLDisplay egl_display = eglGetDisplay((EGLNativeDisplayType) display);
	
	if (egl_display == EGL_NO_DISPLAY) {
		DgLog(DG_LOG_ERROR, "Failed to get EGL display for the native display.");
		return DG_ERROR_FAILED;
	}
	
	this->egl_display = egl_display;
	
	// Initialise EGL for this display connection
	if (!eglInitialize(egl_display, NULL, NULL)) {
		DgLog(DG_LOG_ERROR, "Initialising EGL failed.");
		return DG_ERROR_FAILED;
	}
	
	// Now we can load EGL with the display set.
	egl_version = gladLoaderLoadEGL(egl_display);
	
	if (!egl_version) {
		DgLog(DG_LOG_ERROR, "Failed to load EGL for the native display.");
		return DG_ERROR_FAILED;
	}
	
	// Log version
	const int egl_maj = GLAD_VERSION_MAJOR(egl_version);
	const int egl_min = GLAD_VERSION_MINOR(egl_version);
	
	DgLog(DG_LOG_INFO, "Using EGL %d.%d", egl_maj, egl_min);
	
	// Choose display configuration
	EGLConfig egl_config;
	EGLint egl_config_count;
	EGLint egl_config_attr[] = {
		EGL_BUFFER_SIZE, 16,
		EGL_RENDERABLE_TYPE,
		EGL_OPENGL_ES2_BIT,
		EGL_NONE,
	};
	
	if (!eglChooseConfig(egl_display, egl_config_attr, &egl_config, 1, &egl_config_count)) {
		DgLog(DG_LOG_ERROR, "Could not choose any EGL config.");
		return DG_ERROR_FAILED;
	}
	
	if (egl_config_count != 1) {
		DgLog(DG_LOG_ERROR, "More than one EGL config.");
		return DG_ERROR_FAILED;
	}
	
	this->egl_config = egl_config;
	
	// Create an offscreen EGL surface
	EGLint egl_surface_attr[] = {
		EGL_HEIGHT, size.x,
		EGL_WIDTH, size.y,
		EGL_NONE,
	};
	
	EGLSurface egl_surface = eglCreatePbufferSurface(egl_display, egl_config, egl_surface_attr);
	
	if (egl_surface == EGL_NO_SURFACE) {
		DgLog(DG_LOG_ERROR, "Could not create EGL surface: status code %d", eglGetError());
		return DG_ERROR_FAILED;
	}
	
	this->egl_surface = egl_surface;
	
	// Create EGL context and make it current
	EGLint egl_context_attr[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE,
	};
	
	EGLContext egl_context = eglCreateContext(egl_display, egl_config, EGL_NO_CONTEXT, egl_context_attr);
	
	if (egl_context == EGL_NO_CONTEXT) {
		DgLog(DG_LOG_ERROR, "Could not create EGL context: status code %d", eglGetError());
		return DG_ERROR_FAILED;
	}
	
	this->egl_context = egl_context;
	
	eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
	
	// Finally load OpenGL ES 2.0
	int gl_version = gladLoaderLoadGLES2();
	
	if (!gl_version) {
		return DG_ERROR_FAILED;
	}
	
	DgLog(DG_LOG_INFO, "Using OpenGL ES %d.%d", GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version));
	
	return DG_ERROR_SUCCESS;
}

DgError RoDrawBegin(RoContext * const this) {
	/**
	 * Start the drawing process
	 * 
	 * @note Only really does some basic housekeeping
	 */
	
	return DG_ERROR_NOT_IMPLEMENTED;
}


DgError RoDrawEnd(RoContext * const this) {
	/**
	 * Finish the drawing process and swap front and back buffers
	 */
	
	glClearColor(DgPrandFloat(), DgPrandFloat(), DgPrandFloat(), 1.0);
	glClear(GL_COLOR_BUFFER_BIT);
	
	eglSwapBuffers(this->egl_display, this->egl_surface);
	
	return DG_ERROR_SUCCESS;
}

DgError RoGetFrameData(RoContext * const this, size_t size, void *data, bool alpha) {
	/**
	 * Get the data for the front frame
	 * 
	 * @param this Context
	 * @param size Max size of the buffer
	 * @param data Pointer to the buffer to write into
	 * @param alpha If alpha should be included
	 */
	
	size_t req_size = (alpha ? 4 : 3) * this->size.x * this->size.y;
	
	if (size < req_size) {
		return DG_ERROR_NOT_SAFE;
	}
	
	glReadPixels(0, 0, this->size.x, this->size.y, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
	
	if (glGetError() != GL_NO_ERROR) {
		return DG_ERROR_FAILED;
	}
	
	return DG_ERROR_SUCCESS;
}

void RoContextDestroy(RoContext * const this) {
	/**
	 * Destroy the context
	 */
	
	gladLoaderUnloadGLES2();
	
	eglDestroyContext(this->egl_display, this->egl_context);
	eglDestroySurface(this->egl_display, this->egl_surface);
	eglTerminate(this->egl_display);
	
	gladLoaderUnloadEGL();
	
	XCloseDisplay(this->display);
}
