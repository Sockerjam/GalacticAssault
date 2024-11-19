#pragma once
#include "../Events/Events.h"
#include "../ECS/ESC.h"

class SoundEffectSystem : public System {

public:

	SoundEffectSystem() = default;

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<SoundEffectSystem, SoundEffectEvent>(this, &SoundEffectSystem::playSoundEffect);
	}

	void playSoundEffect(SoundEffectEvent& event) {

		Mix_Chunk* soundFX = event.assetStore->getSoundFX(event.assetid);
		
		Mix_VolumeMusic(MIX_MAX_VOLUME * 0.4);

		Mix_PlayChannel(-1, soundFX, 0);
	}
};
