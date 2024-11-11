#pragma once

#include <glm/glm.hpp>
#include "../ECS/ESC.h"
#include <SDL.h>
#include <memory>

struct RigidBodyComponent {

	glm::vec2 veclocity;

	RigidBodyComponent(glm::vec2 veclocity = glm::vec2(0, 0)) {
		this->veclocity = veclocity;
	}

};

struct TransformComponent {
	glm::vec2 position;
	glm::vec2 scale;
	double rotation;

	TransformComponent() = default;

	TransformComponent(glm::vec2 position, glm::vec2 scale, double rotation) {
		this->position = position;
		this->scale = scale;
		this->rotation = rotation;
	}
};

struct SpriteComponent {

	std::string assetid;
	glm::vec2 size;
	SDL_Rect srcRect;
	bool isFixed;
	
	SpriteComponent(
		std::string assetid = "",
		glm::vec2 size = glm::vec2(0, 0), 
		glm::vec2 srcRect = glm::vec2(0, 0),
		bool isFixed = false) {

		this->assetid = assetid;
		this->size = size;
		this->srcRect = { (int)srcRect.x, (int)srcRect.y, (int)size.x, (int)size.y};
		this->isFixed = isFixed;
	}
};

struct AnimationComponent {

	// Number of frames in the sequence
	int numFrames;
	// Which frame of the sequence are we at
	int currentFrame;
	// How fast should the animation "move"/change frame
	int frameSpeedRate;

	bool isLoop;
	int loopCount;
	int startTime;

	int loopsCompleted = 0;

	AnimationComponent(
		int numFrames = 0,
		int frameSpeedRate = 0,
		bool isLoop = false,
		int loopCount = 0) {
		this->numFrames = numFrames;
		this->currentFrame = 1;
		this->frameSpeedRate = frameSpeedRate;
		this->isLoop = isLoop;
		this->loopCount = loopCount;
		this->startTime = SDL_GetTicks();
	}
};

struct BoxColliderComponent {
	int width;
	int height;
	glm::vec2 offset;

	BoxColliderComponent(
		int width = 0,
		int height = 0,
		glm::vec2 offset = glm::vec2(0, 0)) {

		this->width = width;
		this->height = height;
		this->offset = offset;
	}
};

struct KeyboardControllerComponent {

	glm::vec2 upVeclovity;
	glm::vec2 rightVelocity;
	glm::vec2 downVelocity;
	glm::vec2 leftVelocity;

	KeyboardControllerComponent(
		glm::vec2 upVeclovity = glm::vec2(0, 0),
		glm::vec2 rightVelocity = glm::vec2(0, 0),
		glm::vec2 downVelocity = glm::vec2(0, 0),
		glm::vec2 leftVelocity = glm::vec2(0, 0)) {
		this->upVeclovity = upVeclovity;
		this->rightVelocity = rightVelocity;
		this->downVelocity = downVelocity;
		this->leftVelocity = leftVelocity;
	}
};

struct TrackingComponent {

	Entity* entity;

	TrackingComponent(Entity* entity = nullptr) : entity(entity) {};
};

struct ShieldComponent {

	std::shared_ptr<Entity> entity;

	ShieldComponent() = default;
	ShieldComponent(std::shared_ptr<Entity> entity) : entity(entity) {};
};

struct FieldOfViewComponent {

public:

	glm::vec2 direction;
	double fieldOfView;

	FieldOfViewComponent(glm::vec2 direction = glm::vec2(0, 0), double fieldOfView = 0.0) {
		
		this->direction = direction;
		this->fieldOfView = fieldOfView;
	}

};

struct CameraComponent {
	CameraComponent() = default;
};

struct ProjectileEmitterComponent {

	float speed;
	int repeatFrequency;
	int projectileDuration;
	float hitPercentDamage;
	bool isFriendly;
	int lastEmissionTime;
	bool fire;
	glm::vec2 direction;

	ProjectileEmitterComponent(
		float speed = 0,
		int repeatFrequency = 1000,
		int projecttileDuration = 10000,
		float hitPercentDamage = 0.1f,
		bool isFriendly = false,
		bool fire = false,
		const glm::vec2& direction = glm::vec2(1, 1)
	) {

		this->speed = speed;
		this->repeatFrequency = repeatFrequency;
		this->projectileDuration = projecttileDuration;
		this->hitPercentDamage = hitPercentDamage;
		this->isFriendly = isFriendly;
		this->fire = fire;
		this->lastEmissionTime = SDL_GetTicks();
		this->direction = direction;
	}

};

struct ProjectileComponent {

	int projectileDuration;
	float hitPercentDamage;
	bool isFriendly;
	int startTime;

	ProjectileComponent(
		int projecttileDuration = 10000,
		float hitPercentDamage = 0.1f,
		bool isFriendly = false
	) {
		this->projectileDuration = projecttileDuration;
		this->hitPercentDamage = hitPercentDamage;
		this->isFriendly = isFriendly;
		this->startTime = SDL_GetTicks();
	}
};

struct HealthComponent {

	float health;

	HealthComponent(float health = 1.0f) {
		this->health = health;
	}
};

struct BackgroundComponent {
	
	std::string assetID;
	SDL_FRect background;

	BackgroundComponent(std::string assetID = "", 
		glm::vec2 position = glm::vec2(0, 0), 
		glm::vec2 size = glm::vec2(0, 0)) :
		assetID(assetID),
		background(SDL_FRect{ position.x, position.y, size.x, size.y }) {
	};
};

struct EnemyComponent {

	EnemyComponent() = default;
};

struct ExplosionComponent {

	ExplosionComponent() = default;
};

struct TextLabelComponent {

	std::string assetid;
	glm::vec2 position;
	std::string text;
	SDL_Color textColor;

	TextLabelComponent(
		std::string assetid = "",
		glm::vec2 position = glm::vec2(0, 0),
		std::string text = "",
		SDL_Color textColor = { 255, 255, 255 }) {
		this->assetid = assetid;
		this->position = position;
		this->text = text;
		this->textColor = textColor;
	}
};

struct DamageComponent {

	float hitDamage;

	DamageComponent(float hitDamage = 0) : hitDamage(hitDamage) {};
};

struct HUDComponent {

	enum HUDType {
		POINTS,
		TITLE,
		HEALTH,
		NONE
	};

	std::shared_ptr<TextLabelComponent> textLabelComponent;
	std::string assetid = "";
	glm::vec2 position{0, 0};
	glm::vec2 size{0, 0};
	HUDType type = NONE;

	HUDComponent() = default;

	HUDComponent(TextLabelComponent& textLabelComponent, HUDType type) {
		this->textLabelComponent = std::make_shared<TextLabelComponent>(textLabelComponent);
    this->type = type;
	};

	HUDComponent(std::string assetid, glm::vec2 position, glm::vec2 size, HUDType type) :
		assetid(assetid),
		position(position),
		size(size),
    type(type) {};
};

struct KillPointsComponent {

  int points = 0;

  KillPointsComponent(int points = 0) : points(points) {};
};

struct LifeComponent {

	int lives;

	LifeComponent(int lives = 3) : lives(lives) {};
};
