#pragma once

#include <map>
#include <string>
#include "SDL.h"
#include <vector>
#include "SDL_ttf.h"

class AssetStore {

private:

	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, TTF_Font*> fonts;
	std::vector<int> textureMap;

public:
	AssetStore();
	~AssetStore();

	void clearAssets();
	void addTexture(SDL_Renderer* renderer, const std::string& assetid, const std::string& filePath);
	SDL_Texture* getTexture(const std::string& assetid);

	void addFont(const std::string fontid, const std::string filePath, int fontSize);
	TTF_Font* getFont(const std::string fontid);

};


