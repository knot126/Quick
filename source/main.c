#include "util/melon.h"

#include "rendroar/rendroar.h"

int main(int argc, const char *argv[]) {
	DgInitTime();
	
	DgError err;
	DgBitmap bitmap;
	DgWindow window;
	RoContext roc;
	
	if ((err = DgBitmapInit(&bitmap, (DgVec2I) {1280, 720}, 3))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise bitmap.");
	}
	
	if ((err = DgWindowInit(&window, "OpenGL ES testing", (DgVec2I) {1280, 720}))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise window.");
	}
	
	if ((err = RoContextCreate(&roc, (DgVec2I) {1280, 720}))) {
		DgLog(DG_LOG_ERROR, "Failed to create GLES2 context: <0x%x>.", err);
	}
	
	while (true) {
		RoDrawBegin(&roc);
		
		RoVertex verts[] = {
			(RoVertex) {0.1, 0.1, 1.0, 0.0, 0.0, 255, 0, 0, 255},
			(RoVertex) {0.9, 0.1, 1.0, 0.0, 0.0, 0, 255, 0, 255},
			(RoVertex) {0.5, 0.9, 1.0, 0.0, 0.0, 0, 0, 255, 255},
		};
		
		// DgLog(DG_LOG_VERBOSE, "RoDrawVerts");
		
		if ((err = RoDrawVerts(&roc, 3, verts))) {
			DgLog(DG_LOG_ERROR, "Error while adding verts: %s.", DgErrorString(err));
		}
		
		// DgLog(DG_LOG_VERBOSE, "RoDrawEnd");
		
		if ((err = RoDrawEnd(&roc))) {
			DgLog(DG_LOG_ERROR, "Error while finishing draw: %s.", DgErrorString(err));
		}
		
		// DgLog(DG_LOG_VERBOSE, "RoGetFrameData");
		
		if ((err = RoGetFrameData(&roc, bitmap.chan * bitmap.width * bitmap.height, bitmap.src, false))) {
			DgLog(DG_LOG_ERROR, "Error while getting frame data: %s.", DgErrorString(err));
		}
		
		DgWindowStatus status = DgWindowUpdate(&window, &bitmap);
		
		if (status == DG_WINDOW_SHOULD_CLOSE) {
			break;
		}
	}
	
	RoContextDestroy(&roc);
	DgWindowFree(&window);
	DgBitmapFree(&bitmap);
	
	return 0;
}
