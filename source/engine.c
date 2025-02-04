#include "util/error.h"
#include "util/melon.h"
// #include "util/storage_filesystem.h"
#include "assets.h"
#include "asset_text.h"

#include "engine.h"

Engine *gEngine;

DgError EngineInit(Engine *this, DgArgs *args) {
	DgInitTime();
	
	// DgStorageAddPool(NULL, DgFilesystemCreatePool(NULL, ""));
	
	AssetManagerInit(&this->assman);
	AssetManagerSetSource(&this->assman, ASSET_SOURCE_FOLDER, "assets");
	RegisterTextAssetTypeAndLoader(&this->assman);
	
	DgTableInit(&this->properties);
	
	DgWindowInit(&this->window, "New Engine", (DgVec2I) {1280, 720});
	
	RoContextCreateDW(&this->roc, DgWindowGetNativeDisplayHandle(&this->window), DgWindowGetNativeWindowHandle(&this->window));
	
	this->frames = 0;
	
	return DG_ERROR_SUCCESS;
}

const char *gMainScriptPath = "main.script";

void EngineLoadMainScene(Engine *this) {
	Text mainScriptText = LoadText(&this->assman, gMainScriptPath);
	
	DgLog(DG_LOG_INFO, "Loaded main text asset: %s (size = %d)", gMainScriptPath, mainScriptText->size);
}

DgError EngineRun(Engine *this) {
	DgError err;
	
	EngineLoadMainScene(this);
	
	while (!DgWindowShouldClose(&this->window)) {
		double start = DgTime();
		
		RoDrawBegin(&this->roc);
		
		float t = 2.0 * DgSin(0.25 * start);
		
		RoVertex verts[] = {
			(RoVertex) {-0.5 * t,  0.5 * t, 1.0, 0.0, 0.0, 255, 0, 0, 255},
			(RoVertex) { 0.5 * t,  0.5 * t, 1.0, 0.0, 1.0, 0, 255, 0, 255},
			(RoVertex) { 0.0 * t, -0.5 * t, 1.0, 1.0, 0.0, 0, 0, 255, 255},
		};
		
		if ((err = RoDrawPlainVerts(&this->roc, 3, verts))) {
			DgLog(DG_LOG_ERROR, "Error while adding verts: %s.", DgErrorString(err));
		}
		
		if ((err = RoDrawEnd(&this->roc))) {
			DgLog(DG_LOG_ERROR, "Error while finishing draw: %s.", DgErrorString(err));
		}
		
		DgWindowStatus status = DgWindowUpdate(&this->window, NULL);
		
		if (status == DG_WINDOW_SHOULD_CLOSE) {
			break;
		}
		
		this->frames++;
		
		double delta = (DgTime() - start);
		double sleeptime = (1.0/60.0) - delta;
		
		// DgLog(DG_LOG_INFO, "Frame %d took %g ms, will sleep for %g ms", this->frames, 1000.0 * delta, 1000.0 * sleeptime);
		
		DgSleep(sleeptime);
	}
	
	return DG_ERROR_SUCCESS;
}

int EngineFree(Engine *this) {
	RoContextDestroy(&this->roc);
	DgWindowFree(&this->window);
	
	return 0;
}
