#include "util/error.h"
#include "util/melon.h"
#include "util/storage_filesystem.h"
#include "assets.h"
#include "asset_text.h"

#include "engine.h"

Engine *gEngine;

DgError EngineInit(Engine *this, DgArgs *args) {
	DgInitTime();
	
	DgStorageAddPool(NULL, DgFilesystemCreatePool(NULL, ""));
	DgStorageAddPool(NULL, DgFilesystemCreatePool("assets", DgArgGetValue(args, "assets")));
	
	DgError err;
	
	if ((err = AssetManagerInit(&this->assman))) {
		DgLog(DG_LOG_ERROR, "Failed to init asset manager!");
		return err;
	}
	
	RegisterTextAssetTypeAndLoader(&this->assman);
	
	if ((err = DgTableInit(&this->properties))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise engine properties table.");
		return err;
	}
	
	if ((err = DgWindowInit(&this->window, "OpenGL ES testing", (DgVec2I) {1280, 720}))) {
		DgLog(DG_LOG_ERROR, "Failed to initialise window.");
		return err;
	}
	
	if ((err = RoContextCreateDW(&this->roc, DgWindowGetNativeDisplayHandle(&this->window), DgWindowGetNativeWindowHandle(&this->window)))) {
		DgLog(DG_LOG_ERROR, "Failed to create GLES2 context: <0x%x>.", err);
		return err;
	}
	
	this->frames = 0;
	
	return DG_ERROR_SUCCESS;
}

const char *gMainScript = "main.lua";

DgError EngineLoadMainScene(Engine *this) {
	Text mainScriptText = LoadText(&this->assman, gMainScript);
	
	if (!mainScriptText) {
		DgLog(DG_LOG_ERROR, "Could not load main script");
		return DG_ERROR_FAILED;
	}
	
	DgLog(DG_LOG_INFO, "Loaded main text asset: %s", gMainScript);
	DgLog(DG_LOG_VERBOSE, "Length: %d\n%s", mainScriptText->length, mainScriptText->data);
}

DgError EngineRun(Engine *this) {
	DgError err;
	
	if ((err = EngineLoadMainScene(this))) {
		return err;
	}
	
	while (!DgWindowShouldClose(&this->window)) {
		double start = DgTime();
		
		RoDrawBegin(&this->roc);
		
		RoVertex verts[] = {
			(RoVertex) {-0.5,  0.5, 1.0, 0.0, 0.0, 255, 0, 0, 255},
			(RoVertex) { 0.5,  0.5, 1.0, 0.0, 0.0, 0, 255, 0, 255},
			(RoVertex) { 0.0, -0.5, 1.0, 0.0, 0.0, 0, 0, 255, 255},
		};
		
		if ((err = RoDrawVerts(&this->roc, 3, verts))) {
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
		
		DgLog(DG_LOG_INFO, "Took %g ms, will sleep for %g ms", 1000.0 * delta, 1000.0 * sleeptime);
		
		DgSleep(sleeptime);
	}
	
	return DG_ERROR_SUCCESS;
}

int EngineFree(Engine *this) {
	RoContextDestroy(&this->roc);
	DgWindowFree(&this->window);
	
	return 0;
}
