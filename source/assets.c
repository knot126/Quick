/**
 * Asset manager
 */

#include "assets.h"
#include "common.h"
#include "util/error.h"
#include "util/string.h"
#include "util/table.h"

DgError AssetManagerInit(AssetManager *this) {
	if (DgTableInit(&this->types)) {
		return DG_ERROR_FAILED;
	}
	
	if (DgTableInit(&this->loaders)) {
		return DG_ERROR_FAILED;
	}
	
	if (DgTableInit(&this->assets)) {
		return DG_ERROR_FAILED;
	}
	
	return DG_ERROR_SUCCESS;
}

void AssetManagerFree(AssetManager *this) {
	DgTableFree(&this->types, true);
	DgTableFree(&this->loaders, true);
	DgTableFree(&this->assets, true);
	DgMemoryFree(this->dir);
}

DgError AssetManagerSetSource(AssetManager *this, int source_type, const char *path) {
	/**
	 * Set where to load assets from
	 */
	
	if (source_type == ASSET_SOURCE_FOLDER) {
		this->dir = DgStringDuplicate(path);
		
		if (!this->dir) {
			return DG_ERROR_FAILED;
		}
		else {
			DgLog(DG_LOG_INFO, "Asset directory is now: %s", this->dir);
		}
		
		return DG_ERROR_SUCCESS;
	}
	
	return DG_ERROR_FAILED;
}

DgError AssetManagerAddType(AssetManager *this, AssetType *type) {
	/**
	 * Register an asset type. Note the type's memory must stay allocated, the
	 * AM doesn't make an internal copy of the memory.
	 */
	
	return DgTableSetPointer(&this->types, type->name, (void *) type);
}

DgError AssetManagerAddLoader(AssetManager *this, const char *ext, AssetLoader *loader) {
	/**
	 * Register an asset loader. Note the loader's description must stay
	 * allocated.
	 */
	
	return DgTableSetPointer(&this->loaders, ext ? ext : "", (void *) loader);
}

static Asset AssetManager_GetAlreadyLoadedAsset(AssetManager *this, AssetTypeName type, const char *name) {
	/**
	 * Get an already loaded asset if available and its of the matching type.
	 */
	
	DgLog(DG_LOG_INFO, "AssetManager_GetAlreadyLoadedAsset(<this>, %s, %s)", type, name);
	
	void *asset = DgTableGetPointer(&this->assets, name);
	
	if (asset) {
		Asset_base *ab = (Asset_base *) asset;
		
		if (DgStringEqual(ab->type, type)) {
			ab->refs++;
			return (Asset)(asset + sizeof(Asset_base));
		}
		else {
			return NULL;
		}
	}
	else {
		return NULL;
	}
}

static Asset AssetManager_LoadNewAsset(AssetManager *this, AssetTypeName type, const char *name) {
	/**
	 * Really load an asset, assuming the it doesn't exist already.
	 */
	
	DgLog(DG_LOG_INFO, "AssetManager_LoadNewAsset(<this>, %s, %s)", type, name);
	
	size_t size;
	void *data;
	
	char *path = DgStringConcatinateL(DgStringConcatinate(this->dir, "/"), name);
	
	// Load data
	DgError status = DgFileLoad(path, &size, &data);
	
	if (status) {
		DgLog(DG_LOG_ERROR, "Failed to load asset: %s [%s]", path, DgErrorString(status));
		DgMemoryFree(path);
		return NULL;
	}
	
	DgMemoryFree(path);
	
	// Determine best loader
	AssetLoader *loader = NULL;
	DgValue *key, *value;
	
	for (size_t i = 0; i < DgTableLength(&this->loaders); i++) {
		if (DgTablePairAt(&this->loaders, i, &key, &value)) {
			continue;
		}
		
		AssetLoader *cand = (AssetLoader *) value->data.asPointer;
		
		if (DgStringEqual(cand->type, type) && DgStringEndsWith(name, key->data.asString)) {
			loader = cand;
			break;
		}
	}
	
	if (loader == NULL) {
		DgLog(DG_LOG_ERROR, "Could not find loader for %s", name);
		DgMemoryFree(data);
		return NULL;
	}
	
	// Load the asset
	if (loader->size) {
		Asset_base *ab = DgMemoryAllocate(sizeof(Asset_base) + loader->size);
		ab->type = loader->type;
		ab->refs = 1;
		
		if (loader->load) {
			bool success = loader->load((void *)(ab + 1), size, data);
			
			if (!success) {
				DgLog(DG_LOG_ERROR, "loader->load() failed");
				DgMemoryFree(data);
				return NULL;
			}
			else {
				return (Asset)(ab + 1);
			}
		}
		else {
			DgLog(DG_LOG_ERROR, "loader->load is null");
			DgMemoryFree(data);
			return NULL;
		}
	}
	else {
		if (loader->load_dyn) {
			Asset_base *ab = loader->load_dyn(size, data);
			return ab ? (Asset)(ab + 1) : NULL;
		}
		else {
			DgMemoryFree(data);
			return NULL;
		}
	}
}

Asset AssetManagerLoad(AssetManager *this, AssetTypeName type, const char *name) {
	/**
	 * Try to load an asset from the asset manager.
	 */
	
	Asset asset = AssetManager_GetAlreadyLoadedAsset(this, type, name);
	
	if (asset) {
		return asset;
	}
	
	asset = AssetManager_LoadNewAsset(this, type, name);
	
	if (!asset) {
		DgRaise("ResourceLoadError", "Failed to load a resource");
	}
	
	return asset;
}

static void AssetManager_UnloadAssetByName(AssetManager *this, const char *name) {
	/**
	 * Free an asset
	 */
	
	Asset_base *asset = DgTableGetPointer(&this->assets, name);
	
	if (asset) {
		AssetType *type = DgTableGetPointer(&this->types, asset->type);
		
		if (type->release) {
			type->release(((void *) asset) + sizeof(Asset_base));
		}
		
		DgMemoryFree(asset);
		
		DgValue name_v = DgMakeStaticString(name);
		if (DgTableRemove(&this->assets, &name_v)) {
			DgLog(DG_LOG_WARNING, "Could not remove asset with name %s from assets table, might cause crash later on", name);
		}
	}
}

static const char *AssetManager_FindNameForAsset(AssetManager *this, Asset asset) {
	/**
	 * Find an asset name for the given asset
	 */
	
	return NULL;
}

void AssetManagerRelease(AssetManager *this, Asset asset) {
	/**
	 * Decrement the reference count on the asset
	 */
}
