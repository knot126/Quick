/**
 * Asset manager
 */

#pragma once

#include "common.h"
#include "util/storage.h"

typedef const char *AssetTypeName;

typedef struct AssetType {
	/**
	 * A kind of asset, like Text, Image, Model or Script.
	 */
	
	// The name of the type - it's recommended to be the same as the struct name
	const char *name;
	
	// Function to release resources assocaited with an asset of this type
	// Usually this will cast `asset` to the specific type needed, then
	// free used resources.
	void (*release)(void *asset);
} AssetType;

typedef struct AssetLoader {
	/**
	 * Simple loader for a specific format of asset, usually determined by file
	 * extension.
	 */
	
	// The name of the type this loader loads.
	const char *type;
	
	// The sizeof() the base asset structure. If this is zero, load_dyn is
	// used instead and the structure is dynamically sized.
	size_t size;
	
	union {
		// This function should:
		// - Cast `asset` to (MyCustomAssetType *)
		// - Load the asset from `data`
		// - Return true for success, false on failure
		bool (*load)(void *asset, size_t size, void *data);
		
		// Special version of load() which loads an asset where the base structure
		// is dynamically sized (e.g. not always known at compile time). It is
		// expected to return an Asset_base pointer with the data loaded, though
		// the common Asset_base parts don't need to be properly initialised.
		struct Asset_base *(*load_dyn)(size_t size, void *data);
	};
} AssetLoader;

typedef struct Asset_base {
	/**
	 * Base struct for assets, containing info common to all types of assets.
	 */
	
	const char *type; // String containing the asset type
	uint32_t refs; // Number of references to this asset that exist
	uint32_t _pad16; // HACK does not work on 32bit
} Asset_base;

typedef const void *Asset;

typedef struct AssetManager {
	/**
	 * The asset manager
	 */
	
	DgTable types;
	DgTable loaders;
	DgTable assets;
} AssetManager;

DgError AssetManagerInit(AssetManager *this);
void AssetManagerFree(AssetManager *this);
DgError AssetManagerAddType(AssetManager *this, AssetType *type);
DgError AssetManagerAddLoader(AssetManager *this, const char *ext, AssetLoader *loader);
Asset AssetManagerLoad(AssetManager *this, AssetTypeName type, const char *name);
