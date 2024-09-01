/**
 * Asset manager
 */

#include "assets.h"
#include "common.h"
#include "util/error.h"

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
	
	return DgTableSetPointer(&this->loaders, ext, (void *) loader);
}

static Asset AssetManager_GetAlreadyLoadedAsset(AssetManager *this, const char *name) {
	/**
	 * Get an already loaded asset if available.
	 */
	
	void *asset = DgTableGetPointer(&this->assets, name);
	
	if (asset) {
		return (Asset)(asset + sizeof(Asset_base));
	}
	else {
		return NULL;
	}
}

static Asset AssetManger_LoadNewAsset(AssetManager *this, const char *name) {
	/**
	 * Really load an asset, assuming the it doesn't exist already.
	 */
	
	size_t size;
	void *data;
	
	// Load data
	DgError status = DgStorageLoad(NULL, name, &size, &data);
	
	if (status) {
		DgLog(DG_LOG_ERROR, "Failed to load asset: %s", name);
		return NULL;
	}
	
	// Determine best loader
	AssetLoader *loader = NULL;
	DgValue *key, *value;
	
	for (size_t i = 0; i < DgTableLength(&this->loaders); i++) {
		if (DgTablePairAt(&this->loaders, i, &key, &value)) {
			continue;
		}
		
		AssetLoader *candidate_loader = (AssetLoader *) value->data.asPointer;
		
		if (DgStringEndsWith(name, key->data.asString)) {
			loader = candidate_loader;
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

Asset AssetManagerLoad(AssetManager *this, const char *name) {
	/**
	 * Try to load an asset from the asset manager.
	 */
	
	Asset asset = AssetManager_GetAlreadyLoadedAsset(this, name);
	
	if (asset) {
		return asset;
	}
	
	return AssetManger_LoadNewAsset(this, name);
}
