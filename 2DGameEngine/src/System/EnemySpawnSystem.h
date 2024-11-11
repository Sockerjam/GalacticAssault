#pragma once
#include "../ECS/ESC.h"
#include "../Game/Game.h"
#include "../Components/Components.h"
#include "../Events/Events.h"
#include "../Helpers/Helpers.h"
#include "../Helpers/Colours.h"
#include "../Logger/Logger.h"


class EnemySpawnSystem : public System {

private:

	int turn = 1;
	float speed = 50.0f;
	float speedMultiplier = 1.0f;

	const void spawnTenEnemies(EnemySpawnEvent& event) {

		for (int i = 0; i < 10; i++) {
			float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float randomNr = std::round(random * 10) / 10;
			float offset = i * 100;

			float randomY = randomNr * event.mapHeight;
			float randomX = (randomNr * event.mapWidth) + event.mapWidth + offset;

			int scaledMultiplier = speedMultiplier * 100;
			int decimal = scaledMultiplier % 10;
			std::string enemySpriteName = decimal % 2 == 0 ? "enemyBlack" : "enemyBlue";

			SDL_Rect spriteSize = Helper::getTextureSize(event.assetStore, enemySpriteName);

			Entity enemyShip = event.registry->createEntity(enemy);
			enemyShip.addComponent<TransformComponent>(glm::vec2(randomX, randomY), glm::vec2(1.0f, 1.0f), 0.0f);
			enemyShip.addComponent<RigidBodyComponent>(-glm::vec2(event.speed, 0.0f));
			enemyShip.addComponent<SpriteComponent>(enemySpriteName, glm::vec2(spriteSize.w, spriteSize.h), glm::vec2(spriteSize.x, spriteSize.y), false);
			enemyShip.addComponent<EnemyComponent>();
			enemyShip.addComponent<BoxColliderComponent>(spriteSize.w, spriteSize.h);
			enemyShip.addComponent<HealthComponent>();
			enemyShip.addComponent<ExplosionComponent>();
			enemyShip.addComponent<DamageComponent>(0.5f);
			enemyShip.addComponent<TextLabelComponent>("digiBody", glm::vec2(0, 0), "100%", Color::GREEN);
			enemyShip.addComponent<KillPointsComponent>(1);

		}
	}

	const void spawnAIEnemy(EnemySpawnEvent& event) {

		Entity* playerEntity = event.registry->getPlayerEntity().get();

		float random = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
		float randomNr = std::round(random * 10) / 10;
		float randomY = randomNr * event.mapHeight;
		float randomX = (randomNr * event.mapWidth) + event.mapWidth;

		std::string assetID = "enemyAI";

		SDL_Rect spriteSize = Helper::getTextureSize(event.assetStore, assetID);
		
		Entity enemyAI = event.registry->createEntity(enemy);
		enemyAI.addComponent<TransformComponent>(glm::vec2(randomX, randomY), glm::vec2(1.0f, 1.0f), 0);
		enemyAI.addComponent<RigidBodyComponent>();
		enemyAI.addComponent<SpriteComponent>(assetID, glm::vec2(spriteSize.w, spriteSize.h), glm::vec2(spriteSize.x, spriteSize.y), false);
		enemyAI.addComponent<EnemyComponent>();
		enemyAI.addComponent<TrackingComponent>(playerEntity);
		enemyAI.addComponent<ProjectileEmitterComponent>(50.0f, 600, 5000, 0.1f, false, glm::vec2(-1, 1));

	}

public:

	EnemySpawnSystem() {
		requireComponent<EnemyComponent>();	
		srand(static_cast<Uint32>(time(0)));
	}

	void update(std::unique_ptr<Registry>& registry, 
		std::unique_ptr<EventBus>& eventBus, 
		std::unique_ptr<AssetStore>& assetStore, 
		int mapWidth, 
		int mapHeight) {

		const auto& entities = getEntities();

		if (entities.empty()) {
			speed *= speedMultiplier;
			eventBus->publishEvent<EnemySpawnEvent>(registry, assetStore, mapWidth, mapHeight, speed);
			speedMultiplier += static_cast<float>(turn) / 20.0f;
			turn++;
		}
	};

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<EnemySpawnSystem, EnemySpawnEvent>(this, &EnemySpawnSystem::spawnEnemies);
	}

	void spawnEnemies(EnemySpawnEvent& event) {
		//if (turn % 2 != 0) {
		//	spawnTenEnemies(event);
		//}
		//else {
		//	spawnAIEnemy(event);
		//}
		spawnAIEnemy(event);
	}
};

