#include "./AssetStore.h"
#include "../Logger/Logger.h"
#include <SDL_image.h>

AssetStore::AssetStore()
{
	Logger::Log("AssetStore constructor called");
}

AssetStore::~AssetStore() {
	clearAssets();
	Logger::Log("AssetStore destructor called");
}

void AssetStore::addTexture(SDL_Renderer* renderer, const std::string& assetid, const std::string& filePath) {
	SDL_Surface* surface = IMG_Load(filePath.c_str());

	if (surface == NULL) {
		Logger::LogErr(SDL_GetError());
	}

	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
	
	if (texture == NULL) {
		Logger::LogErr(SDL_GetError());
	}

	SDL_FreeSurface(surface);

	textures.emplace(assetid, texture);

	Logger::Log("New Texture created with id: " + assetid);
}

SDL_Texture* AssetStore::getTexture(const std::string& assetid) {
	return textures[assetid];
}

void AssetStore::clearAssets() {
	for (auto texture : textures) {
		SDL_DestroyTexture(texture.second);
	}
	textures.clear();
	
	for (auto font : fonts) {
		TTF_CloseFont(font.second);
	}
	fonts.clear();
	for (auto sound : sounds) {
		Mix_FreeChunk(sound.second);
	}
	sounds.clear();
}

void AssetStore::addFont(const std::string fontid, const std::string filePath, int fontSize) {
	fonts.emplace(fontid, TTF_OpenFont(filePath.c_str(), fontSize));
}

TTF_Font* AssetStore::getFont(const std::string fontid) {
	return fonts[fontid];
}

void AssetStore::addSound(const std::string assetid, const std::string filePath) {
	
	Mix_Chunk* audio = Mix_LoadWAV(filePath.c_str());

	if (audio == nullptr) {
		Logger::LogErr("Failed To Load Audio at " + filePath);
		return;
	}

	sounds.emplace(assetid, audio);

	Logger::Log("Audio created with id: " + assetid);
}

Mix_Chunk* AssetStore::getSoundFX(const std::string assetid) {
	return sounds[assetid];
}

void AssetStore::addMusic(const std::string assetid, const std::string filePath) {
	
	Mix_Music* track = Mix_LoadMUS(filePath.c_str());

	if (track == nullptr) {
		Logger::LogErr("Failed To Load Audio at " + filePath);
		return;
	}

	music.emplace(assetid, track);

	Logger::Log("Music created with id: " + assetid);
}

Mix_Music* AssetStore::getMusic(const std::string assetid) {
	return music[assetid];
}

