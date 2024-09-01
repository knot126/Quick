#include "assets.h"
#include "asset_text.h"

static void TextAsset_Release(void *asset_text) {
	Text_s *t = (Text_s *) asset_text;
	DgMemoryFree(t->data);
}

AssetType gAtAssetText = {
	.name = "Text",
	.release = TextAsset_Release,
};

static bool GenericTextAsset_Load(void *asset, size_t size, void *data) {
	Text_s *t = (Text_s *) asset;
	t->size = size;
	t->data = data;
	return true;
}

AssetLoader gAlGenericText = {
	.type = "Text",
	.size = sizeof(Text_s),
	.load = GenericTextAsset_Load,
};

void RegisterTextAssetTypeAndLoader(AssetManager *this) {
	AssetManagerAddType(this, &gAtAssetText);
	AssetManagerAddLoader(this, NULL, &gAlGenericText);
}

Text LoadText(AssetManager *this, const char *name) {
	return (Text) AssetManagerLoad(this, "Text", name);
}
