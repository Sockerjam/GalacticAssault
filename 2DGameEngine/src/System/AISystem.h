#pragma once
#include "../Components/Components.h"

class AISystem : public System {

public:

	AISystem() {
		requireComponent<RigidBodyComponent>();
		requireComponent<TrackingComponent>();
		requireComponent<TransformComponent>();
	}

	void update() {

		for (auto& entity : getEntities()) {

			const auto& trackingComponent = entity.getComponent<TrackingComponent>();
			const auto& transformComponent = entity.getComponent<TransformComponent>();
			auto& rigidBodyComponent = entity.getComponent<RigidBodyComponent>();

			if (trackingComponent.entity) {
				const Entity& entityToTrack = *trackingComponent.entity;
				const auto& entityToTrackTransformComponent = entityToTrack.getComponent<TransformComponent>();

				glm::vec2 directionVector = entityToTrackTransformComponent.position - transformComponent.position;

				rigidBodyComponent.veclocity = glm::normalize(directionVector) * 40.0f;
			}
			else {
				Logger::Log("entityToTrack is a nullptr");
			}
		}
	}
};

