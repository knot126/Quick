#include "util/melon.h"
#include "util/storage_filesystem.h"

#include "rendroar/rendroar.h"

int main(int argc, const char *argv[]) {
	DgInitTime();
	
	DgStorageAddPool(NULL, DgFilesystemCreatePool("file", "."));
	
	DgError err;
	DgWindow window;
	RoContext roc;
	
	if ((err = DgWindowInit(&window, "OpenGL ES testing", (DgVec2I) {1280, 720}))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise window.");
	}
	
	if ((err = RoContextCreateDW(&roc, DgWindowGetNativeDisplayHandle(&window), DgWindowGetNativeWindowHandle(&window)))) {
		DgLog(DG_LOG_ERROR, "Failed to create GLES2 context: <0x%x>.", err);
	}
	
	size_t frames = 0;
	
	while (!DgWindowShouldClose(&window)) {
		double start = DgTime();
		
		RoDrawBegin(&roc);
		
		RoVertex verts[] = {
			(RoVertex) {-0.5,  0.5, 1.0, 0.0, 0.0, 255, 0, 0, 255},
			(RoVertex) { 0.5,  0.5, 1.0, 0.0, 0.0, 0, 255, 0, 255},
			(RoVertex) { 0.0, -0.5, 1.0, 0.0, 0.0, 0, 0, 255, 255},
		};
		
		if ((err = RoDrawVerts(&roc, 3, verts))) {
			DgLog(DG_LOG_ERROR, "Error while adding verts: %s.", DgErrorString(err));
		}
		
		if ((err = RoDrawEnd(&roc))) {
			DgLog(DG_LOG_ERROR, "Error while finishing draw: %s.", DgErrorString(err));
		}
		
		DgWindowStatus status = DgWindowUpdate(&window, NULL);
		
		if (status == DG_WINDOW_SHOULD_CLOSE) {
			break;
		}
		
		frames++;
		
		double delta = (DgTime() - start);
		double sleeptime = (1.0/60.0) - delta;
		
		DgLog(DG_LOG_INFO, "Took %g ms, will sleep for %g ms", 1000.0 * delta, 1000.0 * sleeptime);
		
		DgSleep(sleeptime);
	}
	
	DgBitmap bitmap;
	
	if ((err = DgBitmapInit(&bitmap, (DgVec2I) {1280, 720}, 4))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise bitmap.");
	}
	
	if ((err = RoGetFrameData(&roc, bitmap.chan * bitmap.width * bitmap.height, bitmap.src, true))) {
		DgLog(DG_LOG_ERROR, "Error while getting frame data: %s.", DgErrorString(err));
	}
	
	DgBitmapWritePPM(&bitmap, "file://test.ppm");
	
	DgBitmapFree(&bitmap);
	
	RoContextDestroy(&roc);
	DgWindowFree(&window);
	
	return 0;
}
