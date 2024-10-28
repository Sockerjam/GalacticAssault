#pragma once
#include "../Events/Events.h"
#include "../Events/EventBus.h"
#include "../Components/Components.h"
#include "../Assets/AssetStore.h"
#include "../Logger/Logger.h"
#include <glm/glm.hpp>
#include <algorithm>
#include <iterator>
#include <random>
#include <string>
#include "../Helpers/Helpers.h"
#include "../Helpers/Colours.h"


class MovementSystem : public System {

public:

	MovementSystem() {
		requireComponent<TransformComponent>();
		requireComponent<RigidBodyComponent>();
	}

	void update(float deltaTime) {

		for (auto& entity : getEntities()) {

			auto& transform = entity.getComponent<TransformComponent>();
			const auto& rigidBody = entity.getComponent<RigidBodyComponent>();

			transform.position += rigidBody.veclocity * deltaTime;
		}
	}
};

class AnimationSystem : public System {

public:
	AnimationSystem() {
		requireComponent<AnimationComponent>();
		requireComponent<SpriteComponent>();
	}

	void animate() {

		for (auto& entity : getEntities()) {

			auto& spriteComponent = entity.getComponent<SpriteComponent>();
			auto& animationComponent = entity.getComponent<AnimationComponent>();

			animationComponent.currentFrame = ((SDL_GetTicks() - animationComponent.startTime) 
					* animationComponent.frameSpeedRate / 1000) 
					% animationComponent.numFrames;


			spriteComponent.srcRect.x = static_cast<int>(spriteComponent.size.x) * animationComponent.currentFrame;

			if (!animationComponent.isLoop) {

				if (animationComponent.loopsCompleted < animationComponent.loopCount) {

					animationComponent.loopsCompleted++;
				}
				else {
					entity.kill();
				}
				
			}
		}
	}
};

class BoxColliderSystem : public System {

private:

	bool checkCollision(
		TransformComponent& aTransform,
		BoxColliderComponent& aBoxCollider,
		TransformComponent& bTransform,
		BoxColliderComponent& bBoxCollider) {

		int aX = (int)aTransform.position.x + (int)aBoxCollider.offset.x;
		int aY = (int)aTransform.position.y + (int)aBoxCollider.offset.y;
		int aMaxX = aX + aBoxCollider.width;
		int aMaxY = aY + aBoxCollider.height;

		int bX = (int)bTransform.position.x + (int)bBoxCollider.offset.x;
		int bY = (int)bTransform.position.y + (int)bBoxCollider.offset.y;
		int bMaxX = bX + bBoxCollider.width;
		int bMaxY = bY + bBoxCollider.height;

		return (aX < bMaxX && aMaxX > bX && aY < bMaxY && aMaxY > bY);

	}

public:

	BoxColliderSystem() {
		requireComponent<TransformComponent>();
		requireComponent<BoxColliderComponent>();
	}

	void update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry) {

		std::vector<Entity> entities = getEntities();

		for (auto i = entities.begin(); i != entities.end(); i++) {

			Entity a = *i;
			TransformComponent& aTransform = a.getComponent<TransformComponent>();
			BoxColliderComponent& aBoxCollider = a.getComponent<BoxColliderComponent>();

			for (auto j = i + 1; j != entities.end(); j++) {

				Entity b = *j;

				TransformComponent& bTransform = b.getComponent<TransformComponent>();
				BoxColliderComponent& bBoxCollider = b.getComponent<BoxColliderComponent>();

				if (b.hasComponent<ProjectileComponent>()) {
					const auto& projectileComponent = b.getComponent<ProjectileComponent>();
					if (a.getLayer() == player && projectileComponent.isFriendly) {
						return;
					}
					else if (a.getLayer() == player && !projectileComponent.isFriendly) {
						if (checkCollision(aTransform, aBoxCollider, bTransform, bBoxCollider)) {
							eventBus->publishEvent<CollisionEvent>(a, b);
						};
					}
					else if (a.getLayer() == enemy && projectileComponent.isFriendly) {
						if (checkCollision(aTransform, aBoxCollider, bTransform, bBoxCollider)) {
							eventBus->publishEvent<UpdateHealthEvent>(projectileComponent.hitPercentDamage, eventBus, registry, ENEMY, a);
							b.kill();
						}
					}
				}
				else if (a.getLayer() == player || b.getLayer() == player) {
					if (checkCollision(aTransform, aBoxCollider, bTransform, bBoxCollider)) {
						eventBus->publishEvent<CollisionEvent>(a, b);
						eventBus->publishEvent<ExplosionEvent>(registry, PLAYER, a);
					};		
				}
			}
		}
	}
};


class DamageSystem : public System {

public:

	DamageSystem() {
		requireComponent<BoxColliderComponent>();
	}

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<DamageSystem, CollisionEvent>(this, &DamageSystem::onCollisionEvent);
	}

	void onCollisionEvent(CollisionEvent& event) {
		event.a.kill();
		event.b.kill();
	}
};

class ExplosionSystem : public System {

private:

	void createExplosion(std::unique_ptr<Registry>& registry, std::string assetid, glm::vec2 position) {
		Entity playerExplosion = registry->createEntity(explosion);
		playerExplosion.addComponent<TransformComponent>(position, glm::vec2(1, 1), 0);
		playerExplosion.addComponent<SpriteComponent>(assetid, glm::vec2(32, 32));
		playerExplosion.addComponent<AnimationComponent>(4, 10, false, 30);
	}

public:

	ExplosionSystem() = default;

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<ExplosionSystem, ExplosionEvent>(this, &ExplosionSystem::createExplosion);
	}

	void createExplosion(ExplosionEvent& event) {
		
		const auto& transformComponent = event.entity.getComponent<TransformComponent>();
		
		if (event.entity.hasComponent<ExplosionComponent>()) {
		
			switch (event.entityType) {
			case PLAYER:
				createExplosion(event.registry, "playerExplosion", transformComponent.position);
				break;
			case ENEMY:
				createExplosion(event.registry, "enemyExplosion", transformComponent.position);
				break;
			}
		}
	}

};

class KeyboardSystem : public System {

private:

	float angle = 0;
	const float speed = 40;
	glm::vec2 directionVector = { glm::cos(glm::radians(angle)), glm::sin(glm::radians(angle)) };

	void updateDirectionVector() {
		directionVector.x = glm::cos(glm::radians(angle));
		directionVector.y = glm::sin(glm::radians(angle));
	}

public:

	KeyboardSystem() {
		requireComponent<SpriteComponent>();
		requireComponent<RigidBodyComponent>();
		requireComponent<KeyboardControllerComponent>();
		requireComponent<TransformComponent>();
	}

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<KeyboardSystem, KeyboardEvent>(this, &KeyboardSystem::onKeyPressed);
	}

	void onKeyPressed(KeyboardEvent& event) {

		for (auto& entity : getEntities()) {

			auto& rigidBodyComponent = entity.getComponent<RigidBodyComponent>();
			auto& transformComponent = entity.getComponent<TransformComponent>();

			switch (event.symbol) {
			case SDLK_w:
				rigidBodyComponent.veclocity = glm::vec2(0.0f, -1.0f) * speed;
				break;
			case SDLK_s:
				rigidBodyComponent.veclocity = glm::vec2(0.0f, 1.0f) * speed;
				break;
			case SDLK_d:		
				angle += 2;
				transformComponent.rotation = angle;
				updateDirectionVector();
				break;
			case SDLK_a:
				angle -= 2;
				transformComponent.rotation = angle;
				updateDirectionVector();
				break;
			}
		}
	}
};

class AISystem : public System {

public:

	AISystem() {
		requireComponent<RigidBodyComponent>();
		requireComponent<TrackingComponent>();
		requireComponent<TransformComponent>();
	}

	void update(float deltaTime) {

		for (auto& entity : getEntities()) {

			const auto& trackingComponent = entity.getComponent<TrackingComponent>();
			const auto& transformComponent = entity.getComponent<TransformComponent>();
			auto& rigidBodyComponent = entity.getComponent<RigidBodyComponent>();

			if (trackingComponent.entity) {
				const Entity& entityToTrack = *trackingComponent.entity;
				const auto& entityToTrackTransformComponent = entityToTrack.getComponent<TransformComponent>();

				glm::vec2 directionVector = entityToTrackTransformComponent.position - transformComponent.position;

				rigidBodyComponent.veclocity = glm::normalize(directionVector) * 20.0f;
			}
			else {
				Logger::Log("entityToTrack is a nullptr");
			}
		}
	}
};

class StaticEnemySystem : public System {

public:

	StaticEnemySystem() {
		requireComponent<TrackingComponent>();
		requireComponent<FieldOfViewComponent>();
		requireComponent<TransformComponent>();
		requireComponent<SpriteComponent>();
		requireComponent<ProjectileEmitterComponent>();
	}

	void update(std::unique_ptr<EventBus>& eventBus, std::unique_ptr<Registry>& registry, SDL_Keycode symbol) {

		for (auto& entity : getEntities()) {

			const auto& trackingComponent = entity.getComponent<TrackingComponent>();
			const auto& transformComponent = entity.getComponent<TransformComponent>();
			const auto& fieldOfViewComponent = entity.getComponent<FieldOfViewComponent>();
			const auto& spriteComponent = entity.getComponent<SpriteComponent>();

			if (trackingComponent.entity) {

				const Entity& entityToTrack = *trackingComponent.entity;
				const auto& entityToTrackTransformComponent = entityToTrack.getComponent<TransformComponent>();
				const glm::vec2 entityToTrackPosition = entityToTrackTransformComponent.position;
				const auto& entityToTrackSpriteComponent = entityToTrack.getComponent<SpriteComponent>();

				glm::vec2 enemyCenterPosition = transformComponent.position + (spriteComponent.size * 0.5f);
				glm::vec2 entityToTrackCenterPosition = entityToTrackTransformComponent.position + (entityToTrackSpriteComponent.size * 0.5f);

				glm::vec2 enemyToEntityDirectionVector = (entityToTrackCenterPosition - enemyCenterPosition);

				if (glm::length(enemyToEntityDirectionVector) < glm::length(fieldOfViewComponent.direction)) {

					float dotProduct = glm::dot(glm::normalize(enemyToEntityDirectionVector), glm::normalize(fieldOfViewComponent.direction));

					double fieldOfViewProduct = glm::cos(glm::radians(fieldOfViewComponent.fieldOfView * 0.5));

					if (dotProduct > fieldOfViewProduct) {
						eventBus->publishEvent<ProjectileEvent>(registry, symbol);
					}
				}

			}
		}
	}
};

class ProjectileSystem : public System {
public:

		ProjectileSystem() {
			requireComponent<TransformComponent>();
			requireComponent<SpriteComponent>();
			requireComponent<ProjectileEmitterComponent>();
		}

		void subscribeToEvent(std::unique_ptr<EventBus>& eventbus) {
			eventbus->subscribeToEvent<ProjectileSystem, ProjectileEvent>(this, &ProjectileSystem::launchProjectile);
		}

		void launchProjectile(ProjectileEvent& event) {

			const std::vector<Entity>& entities = getEntities();

			std::for_each(entities.begin(), entities.end(), [&](const Entity& entity) {

				if (event.symbol == SDLK_SPACE) {
					if (entity.getLayer() == player) {
						createProjectile(entity, event);
					}
				}
				else if (entity.getLayer() == enemy) {
					createProjectile(entity, event);
				}
				}
			);
		}

		void createProjectile(const Entity& entity, ProjectileEvent& event) {

			const auto& transformComponent = entity.getComponent<TransformComponent>();
			const auto& spriteComponent = entity.getComponent<SpriteComponent>();
			auto& projectileComponent = entity.getComponent<ProjectileEmitterComponent>();

			if (static_cast<int>(SDL_GetTicks()) - projectileComponent.lastEmissionTime > projectileComponent.repeatFrequency) {

				glm::vec2 projectilePosition = Helper::calculcatePosition(transformComponent, spriteComponent);
				
				double radians = glm::radians(transformComponent.rotation);

				glm::vec2 directionVector(glm::cos(radians), glm::sin(radians));

				Entity projectile = event.registry->createEntity(Layer::projectile);

				double rotation = transformComponent.rotation;

				projectile.addComponent<TransformComponent>(
					projectilePosition,
					glm::vec2(1, 1),
					rotation);

				projectile.addComponent<SpriteComponent>("playerLaser");

				glm::vec2 velocity = glm::normalize(directionVector) * projectileComponent.direction * projectileComponent.speed;
				projectile.addComponent<RigidBodyComponent>(velocity);
				projectile.addComponent<BoxColliderComponent>(10, 2);
				projectile.addComponent<ProjectileComponent>(projectileComponent.projectileDuration, projectileComponent.hitPercentDamage, projectileComponent.isFriendly);

				projectileComponent.lastEmissionTime = SDL_GetTicks();
			}
		}
};

class ProjectilLifeTimeSystem : public System {

	public:

		ProjectilLifeTimeSystem() {
			requireComponent<ProjectileComponent>();
		}

		void update() {

			for (auto& entity : getEntities()) {
				auto& projectileComponent = entity.getComponent<ProjectileComponent>();

				if (static_cast<int>(SDL_GetTicks()) - projectileComponent.startTime > projectileComponent.projectileDuration) {
					entity.kill();
				}
			}
		}
};

class EnemySpawnSystem : public System {

private:

	int turn = 1;
	float speed = 50.0f;

public:

	EnemySpawnSystem() {
		requireComponent<EnemyComponent>();	
		srand(static_cast<Uint32>(time(0)));
	}

	void update(std::unique_ptr<Registry>& registry, std::unique_ptr<EventBus>& eventBus, 
             std::unique_ptr<AssetStore>& assetStore, int mapWidth, int mapHeight) {

		const auto& entities = getEntities();

		if (entities.empty()) {
			speed *= turn;
			turn++;
			eventBus->publishEvent<EnemySpawnEvent>(registry, assetStore, mapWidth, mapHeight, speed);
		}
	};

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<EnemySpawnSystem, EnemySpawnEvent>(this, &EnemySpawnSystem::spawnEnemies);
	}

	void spawnEnemies(EnemySpawnEvent& event) {
		
		for (int i = 0; i < 10; i++) {
			float randomNr = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			float randomY = randomNr * event.mapHeight;
			float randomX = (randomNr * event.mapWidth) + event.mapWidth + 100.0f;

			std::string enemySpriteName = "enemyBlack";

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
};

class EnemyBoundsCheckingSystem : public System {

public:

	EnemyBoundsCheckingSystem() {
		requireComponent<EnemyComponent>();
		requireComponent<TransformComponent>();
		requireComponent<SpriteComponent>();
	}

	void update() {
		
		for (auto& entity : getEntities()) {
			const auto& spriteComponent = entity.getComponent<SpriteComponent>();
			const auto& transformComponent = entity.getComponent<TransformComponent>();
			const auto& enemyPosition = transformComponent.position;
			
			if (enemyPosition.x + spriteComponent.size.x < 0) {
				entity.kill();
			}
		}

	}
};

class PointSystem : public System {
private:

  int points = 0;

public:

  PointSystem() {
    requireComponent<HUDComponent>();
  }

  void update() {
    for (auto& entity : getEntities()) {

      auto& hudComponent = entity.getComponent<HUDComponent>();

      if (hudComponent.type == HUDComponent::HUDType::POINTS) {
        if (hudComponent.textLabelComponent != nullptr) {
          hudComponent.textLabelComponent->text = "POINTS: " + std::to_string(points);
        }
      }
    }
  }

  void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
   eventBus->subscribeToEvent<PointSystem, PointEvent>(this, &PointSystem::updatePoints); 
  }

  void updatePoints(PointEvent& event) {
    if (event.entity.hasComponent<KillPointsComponent>()) {
      const auto& killPointsComponent = event.entity.getComponent<KillPointsComponent>();
      points += killPointsComponent.points;
    } 
  }

};

class DynamicTextSystem : public System {
public:

	DynamicTextSystem() {
		requireComponent<TextLabelComponent>();
		requireComponent<TransformComponent>();
		requireComponent<SpriteComponent>();
	}

	void update() {

		for (auto& entity : getEntities()) {

			const auto& transformComponent = entity.getComponent<TransformComponent>();
			auto& textLabelComponent = entity.getComponent<TextLabelComponent>();
			const auto& spriteComponent = entity.getComponent<SpriteComponent>();

			float verticalTextOffset = 2;

			glm::vec2 newTextPosition = {
				transformComponent.position.x,
				transformComponent.position.y + spriteComponent.size.y + verticalTextOffset
			};

			textLabelComponent.position = newTextPosition;
		}
	}

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<DynamicTextSystem, UpdateTextEvent>(this, &DynamicTextSystem::updateText);
	}

	void updateText(UpdateTextEvent& event) {
		
		if (event.entity.hasComponent<TextLabelComponent>()) {
			
			auto& textLabelComponent = event.entity.getComponent<TextLabelComponent>();

			int newHealth = static_cast<int>(std::round(event.health * 100));

			std::string finalHealth = std::to_string(newHealth) + "%";

			textLabelComponent.text = finalHealth;

			if (event.health < 0.5f) {
				textLabelComponent.textColor = Color::ORANGE;
			}

			if (newHealth < 0.2f) {
				textLabelComponent.textColor = Color::RED;
			}
		}
	}
};


class HealthUpdateSystem : public System {

public:
	
	HealthUpdateSystem() = default;

	void subscribeToEvent(std::unique_ptr<EventBus>& eventBus) {
		eventBus->subscribeToEvent<HealthUpdateSystem, UpdateHealthEvent>(this, &HealthUpdateSystem::updateHealth);
	}

	void updateHealth(UpdateHealthEvent& event) {

		auto& healthComponent = event.entity.getComponent<HealthComponent>();

		healthComponent.health -= event.damagePercentage;

		if (event.entity.hasComponent<DamageComponent>()) {
			auto& damageComponent = event.entity.getComponent<DamageComponent>();
			healthComponent.health -= damageComponent.hitDamage;
		}

		event.eventBus->publishEvent<UpdateTextEvent>(event.entity, healthComponent.health);

		if (healthComponent.health <= 0) {
			event.entity.kill();
			event.eventBus->publishEvent<ExplosionEvent>(event.registry, event.entityType, event.entity);
      if (event.entityType == ENEMY) {
        event.eventBus->publishEvent<PointEvent>(event.entity);
      }
		}
	}
};

