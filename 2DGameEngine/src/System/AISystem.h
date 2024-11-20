#pragma once
#include "../Components/Components.h"
#include <glm/glm.hpp>

class AISystem : public System {

private:

	void launchProjectile(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, ProjectileEmitterComponent& projectileComponent) {
		if (static_cast<int>(SDL_GetTicks()) - projectileComponent.lastEmissionTime > projectileComponent.repeatFrequency) {
			eventBus->publishEvent<ProjectileEvent>(registry, SDLK_UNKNOWN);
			eventBus->publishEvent<SoundEffectEvent>(assetStore, "enemyLaser");
		}
	}

public:

	AISystem() {
		requireComponent<RigidBodyComponent>();
		requireComponent<TrackingComponent>();
		requireComponent<TransformComponent>();
		requireComponent<SpriteComponent>();
		requireComponent<ProjectileEmitterComponent>();
	}

	void update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, int mapWidth) {

		for (auto& entity : getEntities()) {

			const auto& trackingComponent = entity.getComponent<TrackingComponent>();
			const auto& spriteSize = entity.getComponent<SpriteComponent>().size;
			auto& projectileEmitterComponent = entity.getComponent<ProjectileEmitterComponent>();
			auto& rigidBodyComponent = entity.getComponent<RigidBodyComponent>();
			auto& transformComponent = entity.getComponent<TransformComponent>();

			if (trackingComponent.entity) {
				const Entity& playerEntity = *trackingComponent.entity;
				const auto& playerEntityTransformComponent = playerEntity.getComponent<TransformComponent>();
				const auto& playerSize = playerEntity.getComponent<SpriteComponent>().size;
				
				glm::vec2 playerCenterPoint = playerEntityTransformComponent.position + (playerSize * 0.5f);
				glm::vec2 entityCenterPoint = transformComponent.position + (spriteSize * 0.5f);

				glm::vec2 directionVector = playerCenterPoint - entityCenterPoint;
				glm::vec2 normalisedDirectionVector = glm::normalize(directionVector);

				double radians = glm::atan(normalisedDirectionVector.y, normalisedDirectionVector.x);
				double degrees = glm::degrees(radians);

				double entityFacingDirection = glm::degrees(std::acos(-1));

				double finalRotation = degrees + entityFacingDirection;
				
				rigidBodyComponent.veclocity = normalisedDirectionVector * rigidBodyComponent.speed;
				transformComponent.rotation = finalRotation;

				if (transformComponent.position.x < mapWidth) {
					launchProjectile(eventBus, registry, assetStore, projectileEmitterComponent);
				}

			}
			else {
				Logger::Log("entityToTrack is a nullptr");
			}
		}
	}
};

