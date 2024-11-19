#pragma once
#include "../Assets/AssetStore.h"
#include "../ECS/ESC.h"

class BackgroundMusicSystem : public System {

public:

	BackgroundMusicSystem() = default;

	void start(std::string assetid, std::unique_ptr<AssetStore>& assetStore) {

		Mix_Music* music = assetStore->getMusic(assetid);

		Mix_VolumeMusic(MIX_MAX_VOLUME / 6);

		Mix_FadeInMusic(music, -1, 10000);
	}

};
