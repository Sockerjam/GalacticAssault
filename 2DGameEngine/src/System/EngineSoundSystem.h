#pragma once
#include "../ECS/ESC.h"
#include "../Assets/AssetStore.h"
#include "../Events/Events.h"


class EngineSoundSystem : public System {

public:

	EngineSoundSystem() = default;

	void start(std::unique_ptr<AssetStore>& assetStore) {
		Mix_Chunk* soundFX = assetStore->getSoundFX("engine");
		
		Mix_VolumeChunk(soundFX, MIX_MAX_VOLUME / 6);
		Mix_FadeInChannel(-1, soundFX, -1, 10000);
	}

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<EngineSoundSystem, StopEngineEvent>(this, &EngineSoundSystem::stop);
	}

	void stop(StopEngineEvent& event) {

		Mix_Chunk* soundFX = event.assetStore->getSoundFX("engine");

		Mix_FreeChunk(soundFX);
	}
};
