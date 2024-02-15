#include "util/melon.h"
#include "util/storage_filesystem.h"

#include "rendroar/rendroar.h"

#define DONT_WINDOW 1

int main(int argc, const char *argv[]) {
	DgInitTime();
	
	DgStorageAddPool(NULL, DgFilesystemCreatePool("file", "."));
	
	DgError err;
	DgBitmap bitmap;
	DgWindow window;
	RoContext roc;
	
	if ((err = DgBitmapInit(&bitmap, (DgVec2I) {1280, 720}, 3 + DONT_WINDOW))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise bitmap.");
	}
	
	if ((err = RoContextCreate(&roc, (DgVec2I) {1280, 720}))) {
		DgLog(DG_LOG_ERROR, "Failed to create GLES2 context: <0x%x>.", err);
	}
	
#if !DONT_WINDOW
	if ((err = DgWindowInit(&window, "OpenGL ES testing", (DgVec2I) {1280, 720}))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise window.");
	}
#endif
	
	size_t frames = 0;
	
	while (frames < 1) {
		RoDrawBegin(&roc);
		
		RoVertex verts[] = {
			(RoVertex) {-0.9,  0.9, 1.0, 0.0, 0.0, 255, 0, 0, 255},
			(RoVertex) { 0.9,  0.9, 1.0, 0.0, 0.0, 0, 255, 0, 255},
			(RoVertex) { 0.0, -0.9, 1.0, 0.0, 0.0, 0, 0, 255, 255},
		};
		
		if ((err = RoDrawVerts(&roc, 3, verts))) {
			DgLog(DG_LOG_ERROR, "Error while adding verts: %s.", DgErrorString(err));
		}
		
		if ((err = RoDrawEnd(&roc))) {
			DgLog(DG_LOG_ERROR, "Error while finishing draw: %s.", DgErrorString(err));
		}
		
		if ((err = RoGetFrameData(&roc, bitmap.chan * bitmap.width * bitmap.height, bitmap.src, DONT_WINDOW))) {
			DgLog(DG_LOG_ERROR, "Error while getting frame data: %s.", DgErrorString(err));
		}
		
#if !DONT_WINDOW
		DgWindowStatus status = DgWindowUpdate(&window, &bitmap);
		
		if (status == DG_WINDOW_SHOULD_CLOSE) {
			break;
		}
#endif
		
		// while(true);
		frames++;
	}
	
	DgBitmapWritePPM(&bitmap, "file://test.ppm");
	
	RoContextDestroy(&roc);
#if !DONT_WINDOW
	DgWindowFree(&window);
#endif
	DgBitmapFree(&bitmap);
	
	return 0;
}
