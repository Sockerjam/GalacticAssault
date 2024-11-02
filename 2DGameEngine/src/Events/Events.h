#pragma once
#include "../ECS/ESC.h"
#include "Event.h"
#include "../Assets/AssetStore.h"
#include <SDL.h>
#include <cwchar>
#include "../Game/Game.h"

class CollisionEvent : public Event {

public:

	Entity a;
	Entity b;

	CollisionEvent(Entity a, Entity b) : a(a), b(b) {};
};

class KeyboardEvent : public Event {

public:

	SDL_Keycode symbol;

	KeyboardEvent(SDL_Keycode symbol) : symbol(symbol) {};
};

class ProjectileEvent : public Event {

public:

	std::unique_ptr<Registry>& registry;
	SDL_Keycode symbol;

	ProjectileEvent(std::unique_ptr<Registry>& registry, SDL_Keycode symbol) : registry(registry), symbol(symbol) {};

};

class EnemySpawnEvent : public Event {

public:

	std::unique_ptr<Registry>& registry;
	std::unique_ptr<AssetStore>& assetStore;
	int mapWidth; 
	int mapHeight;
	float speed;
	
	EnemySpawnEvent(std::unique_ptr<Registry>& registry, std::unique_ptr<AssetStore>& assetStore, int mapWidth, int mapHeight, float speed) : 
		registry(registry),
		assetStore(assetStore),
		mapWidth(mapWidth),
		mapHeight(mapHeight),
		speed(speed) {};
};

class UpdateHealthEvent : public Event {

public:

	float damagePercentage;
	std::unique_ptr<EventBus>& eventBus;
	std::unique_ptr<Registry>& registry; 
	EntityType entityType;
	Entity entity;

	
	UpdateHealthEvent(
		float damagePercentage, 
		std::unique_ptr<EventBus>& eventBus,
		std::unique_ptr<Registry>& registry,
		EntityType entityType,
		Entity entity) : 
		damagePercentage(damagePercentage), 
		eventBus(eventBus),
		registry(registry),
		entityType(entityType),
		entity(entity) {};
};

class ExplosionEvent : public Event {

public:

	std::unique_ptr<Registry>& registry;
	EntityType entityType;
	const Entity& entity;

	ExplosionEvent(
		std::unique_ptr<Registry>& registry,
		EntityType entityType,
		const Entity& entity) :
		registry(registry),
		entityType(entityType),
		entity(entity) {};

};

class UpdateTextEvent : public Event {

public:

	Entity& entity;
	float health;

	UpdateTextEvent(Entity& entity, float health) : entity(entity), health(health) {};
};

class PointEvent : public Event {
public:

  Entity& entity;

  PointEvent(Entity& entity) : entity(entity) {};
};

class LifeLostEvent : public Event {

public:

	int lifeLost;
	Entity& playerEntity;
	Entity& otherEntity;
	std::unique_ptr<EventBus>& eventBus;
	std::unique_ptr<Registry>& registry;

	LifeLostEvent(
		int lifeLost, 
		Entity& playerEntity, 
		Entity& otherEntity,
		std::unique_ptr<EventBus>& eventBus,
		std::unique_ptr<Registry>& registry) : 
		lifeLost(lifeLost),
		playerEntity(playerEntity),
		otherEntity(otherEntity),
		eventBus(eventBus),
		registry(registry){};

};
