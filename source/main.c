#include "util/melon.h"
#include "util/log.h"
#include "util/time.h"
#include "util/pseudorandom.h"

int main(int argc, const char *argv[]) {
	DgInitTime();
	
	DgWindow window;
	
	uint32_t status = DgWindowInit(&window, "Game Engine", (DgVec2I) {1280, 720});
	
	if (status) {
		DgLog(DG_LOG_ERROR, "There was a problem opening the window");
		return 1;
	}
	
	DgBitmap bitmap;
	
	// TODO fix bitmap so it uses DgError's
	status = DgBitmapInit(&bitmap, 1280, 720, 3);
	
	if (status) {
		DgLog(DG_LOG_ERROR, "Could not init bitmap");
		return 1;
	}
	
	DgVec2 mouse_prev;
	DgVec2 mouse_cur = (DgVec2) {0.0f, 0.0f};
	
	while (true) {
		mouse_prev = mouse_cur;
		mouse_cur = DgWindowGetMouseLocation(&window);
		
		if (DgWindowGetMouseDown(&window)) {
			DgBitmapDrawLine(&bitmap, mouse_prev, mouse_cur, &(DgColour) {1.0f, 1.0f, 1.0f, 1.0f});
		}
		
		// TODO fix this so it has some proper return codes
		uint32_t state = DgWindowUpdate(&window, &bitmap);
		
		if (state == 1) {
			break;
		}
	}
	
	DgBitmapFree(&bitmap);
	
	DgWindowFree(&window);
	
	return 0;
}
