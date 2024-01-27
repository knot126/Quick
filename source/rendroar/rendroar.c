#include "util/melon.h"

#include "glad.h"

#include "rendroar.h"

DgError RoContextCreate(RoContext * const context) {
	/**
	 * Create a new context
	 * 
	 * @warning You can only have one context at a time atm.
	 * 
	 * @param context context
	 * @return error while initialising context
	 */
	
	int gl_version = gladLoaderLoadGL();
	
	if (!gl_version) {
		return DG_ERROR_FAILURE;
	}
	
	// Fill out API info
	context->api = RO_API_GLES;
	context->major = GLAD_VERSION_MAJOR(gl_version);
	context->minor = GLAD_VERISON_MINOR(gl_version);
	
	DgLog(DG_LOG_INFO, "Using OpenGL %d.%d", context->major, context->minor);
	
	return DG_ERROR_SUCCESS;
}

void RoContextDestroy(RoContext * const context) {
	/**
	 * Destroy the context
	 */
	
	gladLoaderUnloadGL();
}
