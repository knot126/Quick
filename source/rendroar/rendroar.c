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

#include "ro_program.part"

const char *gRoDefaultShader = "varying vec2 fTextureCoords;\nvarying vec4 fColour;\n\n#ifdef VERTEX\nattribute vec3 inPosition;\nattribute vec2 inTextureCoords;\nattribute vec4 inColour;\n\nvoid main() {\n\tgl_Position = vec4(inPosition, 1.0);\n\tfTextureCoords = inTextureCoords;\n\tfColour = inColour;\n}\n#endif\n\n#ifdef FRAGMENT\nvoid main() {\n\tgl_FragColor = fColour;\n}\n#endif\n";

const RoOpenGLProgramLayoutFeild gRoDefaultShaderLayoutFeilds[] = {
	(RoOpenGLProgramLayoutFeild) {
		.offset = 0,
		.name = "inPosition",
		.components = 3,
		.type = GL_FLOAT,
		.normalise = GL_FALSE,
	},
	(RoOpenGLProgramLayoutFeild) {
		.offset = sizeof(float) * 3,
		.name = "inTextureCoords",
		.components = 2,
		.type = GL_FLOAT,
		.normalise = GL_FALSE,
	},
	(RoOpenGLProgramLayoutFeild) {
		.offset = sizeof(float) * 5,
		.name = "inColour",
		.components = 3,
		.type = GL_UNSIGNED_BYTE,
		.normalise = GL_TRUE,
	},
};

const RoOpenGLProgramLayout *gRoDefaultShaderLayout = &(RoOpenGLProgramLayout) {
	.vertex_size = sizeof(RoVertex),
	.feild_count = 3,
	.feilds = gRoDefaultShaderLayoutFeilds
};

static DgError RoContextCreate_InitGL(RoContext * const this, DgVec2I size) {
	/**
	 * Initialise OpenGL for the new context
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
	
	if (eglGetCurrentContext() != EGL_NO_CONTEXT) {
		DgLog(DG_LOG_WARNING, "A current EGL context is already set.");
	}
	
	eglMakeCurrent(egl_display, egl_surface, egl_surface, egl_context);
	
	EGLint egl_error = eglGetError();
	
	if (egl_error != EGL_SUCCESS) {
		DgLog(DG_LOG_ERROR, "EGL error 0x%x while setting context as current for first time", egl_error);
		return DG_ERROR_FAILED;
	}
	
	// Finally load OpenGL ES 2.0
	int gl_version = gladLoaderLoadGLES2();
	
	if (!gl_version) {
		return DG_ERROR_FAILED;
	}
	
	DgLog(DG_LOG_INFO, "Using OpenGL ES %d.%d", GLAD_VERSION_MAJOR(gl_version), GLAD_VERSION_MINOR(gl_version));
	DgLog(DG_LOG_INFO, "Graphics hardware: %s %s", glGetString(GL_VENDOR), glGetString(GL_RENDERER));
	
	return DG_ERROR_SUCCESS;
}

DgError RoContextCreate(RoContext * const this, DgVec2I size) {
	/**
	 * Create a new context
	 * 
	 * @param this Context object
	 * @param size Size of the image
	 * @return Error code while initialising
	 */
	
	DgError status = RoContextCreate_InitGL(this, size);
	
	if (status) {
		return status;
	}
	
	this->program = DgAlloc(sizeof this->program);
	
	if (!this->program) {
		return DG_ERROR_ALLOCATION_FAILED;
	}
	
	status = RoOpenGLProgramInit(this->program, gRoDefaultShaderLayout, gRoDefaultShader);
	
	if (status) {
		DgLog(DG_LOG_ERROR, "Failed to initialise program, status <0x%x>.", status);
		return status;
	}
	
	this->verticies = DgMemoryStreamCreate();
	
	if (!this->verticies) {
		return DG_ERROR_FAILED;
	}
	
	this->indexes = DgMemoryStreamCreate();
	
	if (!this->indexes) {
		return DG_ERROR_FAILED;
	}
	
	this->background = (DgColour) {0.5, 0.5, 0.5, 1.0};
	
	return DG_ERROR_SUCCESS;
}

void RoContextDestroy(RoContext * const this) {
	/**
	 * Destroy the context
	 */
	
	RoOpenGLProgramFree(this->program);
	DgFree(this->program);
	
	gladLoaderUnloadGLES2();
	
	eglDestroyContext(this->egl_display, this->egl_context);
	eglDestroySurface(this->egl_display, this->egl_surface);
	eglTerminate(this->egl_display);
	
	gladLoaderUnloadEGL();
	
	XCloseDisplay(this->display);
}

void RoContextMakeCurrent(RoContext *this) {
	eglMakeCurrent(this->egl_display, this->egl_surface, this->egl_surface, this->egl_context);
	
	EGLint egl_error;
	
	if ((egl_error = eglGetError()) != EGL_SUCCESS) {
		DgLog(DG_LOG_ERROR, "EGL error 0x%x while setting context as current", egl_error);
	}
}

DgError RoDrawBegin(RoContext * const this) {
	/**
	 * Start the drawing process
	 * 
	 * @note Only really does some basic housekeeping
	 */
	
	DgMemoryStreamRewind(this->verticies);
	DgMemoryStreamRewind(this->indexes);
	
	this->vertex_count = 0;
	this->index_count = 0;
	
	return DG_ERROR_SUCCESS;
}

DgError RoDrawVerts(RoContext * const this, size_t count, RoVertex *verticies) {
	/**
	 * Append the given vertex data to the stream
	 * 
	 * @param this Context
	 * @param count Number of verticies
	 * @param verticies Vertex data
	 * @return Error while drawing verticies
	 */
	
	DgMemoryStreamWrite(this->verticies, sizeof *verticies * count, verticies);
	
	if (DgMemoryStreamError(this->verticies) != DG_MEMORY_STREAM_OKAY) {
		return DG_ERROR_FAILED;
	}
	
	this->vertex_count += count;
	
	return DG_ERROR_SUCCESS;
}

DgError RoDrawEnd(RoContext * const this) {
	/**
	 * Finish the drawing process and swap front and back buffers
	 */
	
	GLenum gl_error = glGetError();
	
	if (gl_error != GL_NO_ERROR) {
		DgLog(DG_LOG_ERROR, "Not drawing due to previous unhandled OpenGL error: <0x%x>", gl_error);
		return DG_ERROR_FAILED;
	}
	
	RoContextMakeCurrent(this);
	
	glViewport(0, 0, this->size.x, this->size.y);
	
	// Clear the screen
	glClearColor(this->background.r, this->background.g, this->background.b, this->background.a);
	glClear(GL_COLOR_BUFFER_BIT);
	
	// Set the active program
	GLuint program = this->program->program;
	glUseProgram(program);
	
	// Setup data stuff
	RoVertex *buffer;
	DgMemoryStreamGetPointersAndSize(this->verticies, NULL, (void *) &buffer);
	
	DgLog(DG_LOG_INFO, "Will draw %d verts", this->vertex_count);
	
	GLint inPosition = glGetAttribLocation(program, "inPosition");
	
	if (inPosition >= 0) {
		glEnableVertexAttribArray(inPosition);
		glVertexAttribPointer(
			inPosition,
			3,
			GL_FLOAT,
			GL_FALSE,
			sizeof(RoVertex),
			&buffer[0].x
		);
	}
	
	GLint inTextureCoords = glGetAttribLocation(program, "inTextureCoords");
	
	if (inTextureCoords >= 0) {
		glEnableVertexAttribArray(inTextureCoords);
		glVertexAttribPointer(
			inTextureCoords,
			2,
			GL_FLOAT,
			GL_FALSE,
			sizeof(RoVertex),
			&buffer[0].u
		);
	}
	
	GLint inColour = glGetAttribLocation(program, "inColour");
	
	if (inColour >= 0) {
		glEnableVertexAttribArray(inColour);
		glVertexAttribPointer(
			inColour,
			4,
			GL_UNSIGNED_BYTE,
			GL_TRUE,
			sizeof(RoVertex),
			&buffer[0].r
		);
	}
	
	// Draw the arrays
	glDrawArrays(GL_TRIANGLES, 0, this->vertex_count);
	
	// Undo setup
	if (inPosition >= 0) glDisableVertexAttribArray(inPosition);
	if (inTextureCoords >= 0) glDisableVertexAttribArray(inTextureCoords);
	if (inColour >= 0) glDisableVertexAttribArray(inColour);
	
	gl_error = glGetError();
	
	if (gl_error != GL_NO_ERROR) {
		DgLog(DG_LOG_ERROR, "Did not draw sucessfully: <0x%x>", gl_error);
		return DG_ERROR_FAILED;
	}
	
	eglSwapBuffers(this->egl_display, this->egl_surface);
	
	EGLint egl_error = eglGetError();
	
	if (egl_error != EGL_SUCCESS) {
		DgLog(DG_LOG_ERROR, "EGL error 0x%x while swapping buffers", egl_error);
		return DG_ERROR_FAILED;
	}
	
	return DG_ERROR_SUCCESS;
}

DgError RoGetFrameData(RoContext * const this, size_t size, void *data, bool alpha) {
	/**
	 * Get the data for the front frame
	 * 
	 * @warning OpenGL ES does not support having alpha be false
	 * 
	 * @warning Due to how OpenGL handles textures the result will be flipped
	 * upside down.
	 * 
	 * @param this Context
	 * @param size Max size of the buffer
	 * @param data Pointer to the buffer to write into
	 * @param alpha If alpha should be included
	 */
	
	size_t req_size = (alpha ? 4 : 3) * this->size.x * this->size.y;
	
	if (size < req_size || !data) {
		return DG_ERROR_NOT_SAFE;
	}
	
	glReadPixels(0, 0, this->size.x, this->size.y, alpha ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
	
	GLenum error = glGetError();
	
	if (error != GL_NO_ERROR) {
		DgLog(DG_LOG_ERROR, "OpenGL error <0x%x> while getting frame data.", error);
		return DG_ERROR_FAILED;
	}
	
	return DG_ERROR_SUCCESS;
}
