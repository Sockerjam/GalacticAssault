#pragma once

#include <map>
#include <string>
#include <SDL.h>
#include <vector>
#include <SDL_ttf.h>
#include <SDL_mixer.h>

class AssetStore {

private:

	std::map<std::string, SDL_Texture*> textures;
	std::map<std::string, TTF_Font*> fonts;
	std::map<std::string, Mix_Chunk*> sounds;
	std::map<std::string, Mix_Music*> music;
	std::vector<int> textureMap;

public:
	AssetStore();
	~AssetStore();

	void clearAssets();
	void addTexture(SDL_Renderer* renderer, const std::string& assetid, const std::string& filePath);
	SDL_Texture* getTexture(const std::string& assetid);

	void addFont(const std::string fontid, const std::string filePath, int fontSize);
	TTF_Font* getFont(const std::string fontid);

	void addSound(const std::string assetid, const std::string filePath);
	Mix_Chunk* getSoundFX(const std::string assetid);

	void addMusic(const std::string assetid, const std::string filePath);
	Mix_Music* getMusic(const std::string assetid);
};


