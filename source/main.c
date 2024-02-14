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
		RoDrawEnd(&roc);
		
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
