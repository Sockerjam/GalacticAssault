#include "Game.h"
#include "../Logger/Logger.h"
#include "../Components/Components.h"
#include "../System/Systems.h"
#include "../System/RenderSystems.h"
#include <glm/glm.hpp>
#include <fstream>
#include <sstream>
#include "../Helpers/Colours.h"
#include "../Helpers/Helpers.h"
#include "../Helpers/Styling.h"

int Game::windowHeight = 720;
int Game::windowWidth = 1024;
int Game::mapHeight = 688;
int Game::mapWidth = 1024;
int Game::mapOffset = Game::windowHeight - Game::mapHeight;

Game::Game()
	: isRunning(false),
	msPreviousFrame(0),
	deltaTime(0),
	displayMode(SDL_DisplayMode()),
	window(SDL_CreateWindow(
		NULL,
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		Game::windowWidth,
		Game::windowHeight,
		SDL_WINDOW_BORDERLESS)),
	renderer(SDL_CreateRenderer(
		window,
		-1,
		SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC)),
	registry(std::make_unique<Registry>()),
	assetStore(std::make_unique<AssetStore>()),
	eventBus(std::make_unique<EventBus>()),
	isDebug(false)
{
	Logger::Log("Game Object Created");

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		Logger::LogErr("Error Initialising SDL");
		return;
	}

	if (TTF_Init() != 0) {
		Logger::LogErr(SDL_GetError());
		return;
	}

	SDL_GetCurrentDisplayMode(0, &displayMode);

	SDL_RenderSetLogicalSize(renderer, windowWidth, windowHeight);
	
	if (!window)
	{
		Logger::LogErr("Error creating SDL window");
	}

	if (!renderer)
	{
		Logger::LogErr("Error creating SDL Renderer");
	};

	//SDL_SetWindowFullscreen(
	//	window, 
	//	SDL_WINDOW_FULLSCREEN);
	
	setCenterValues();
	setup();

	isRunning = true;
};

Game::~Game()
{
	Logger::Log("Game Object Deconstructed");
};

void Game::setCenterValues() {
	centerX = static_cast<float>(Game::mapWidth) * 0.5f;
	centerY = static_cast<float>(Game::mapHeight) * 0.5f;
}

void Game::loadLevel(int level) {

	addSystems();
	addTextures();
	addFonts();
	createBackground();
	createPlayer();
	createHUDComponents();
}

void Game::createBackground() {
	Entity background = registry->createEntity(tileMap);
	background.addComponent<BackgroundComponent>("space", glm::vec2(0.0f, 0.0f), glm::vec2(mapWidth, mapHeight));
}

void Game::createPlayer() {

	float startingX = static_cast<float>(Game::mapWidth) * 0.02f;

	Entity playerShip = registry->createEntity(player);
	playerShip.addComponent<SpriteComponent>("player", glm::vec2(24, 36));
	playerShip.addComponent<TransformComponent>(glm::vec2(startingX, centerY), glm::vec2(1.0f, 1.0f), 0.0f);
	playerShip.addComponent<RigidBodyComponent>(glm::vec2(0.0f, 0.0f));
	playerShip.addComponent<BoxColliderComponent>(24, 36);
	playerShip.addComponent<ProjectileEmitterComponent>(50.0f, 200, 10000, 0.1f, true);
	playerShip.addComponent<KeyboardControllerComponent>();
	playerShip.addComponent<HealthComponent>();
	playerShip.addComponent<TextLabelComponent>("digiBody", glm::vec2(0, 0), "100%", Color::GREEN);
	playerShip.addComponent<LifeComponent>(3);
	playerShip.addComponent<ExplosionComponent>();
	registry->setPlayerEntity(playerShip);

}

void Game::createHUDComponents() {

	Entity title = registry->createEntity(gui);
	TextLabelComponent textLabelComponent("digiBold", glm::vec2(centerX, 0.0f), "GALACTIC ASSAULT", Color::GREEN);
	Helper::centerText(assetStore, renderer, textLabelComponent);
	title.addComponent<HUDComponent>(textLabelComponent, HUDComponent::HUDType::TITLE);

	Entity points = registry->createEntity(gui);
	TextLabelComponent pointTextLabelComponent("digiBold", glm::vec2(Game::windowWidth, 0.0f), "POINTS: 00", Color::GREEN);
	Helper::alignRight(assetStore, renderer, pointTextLabelComponent);
	points.addComponent<HUDComponent>(pointTextLabelComponent, HUDComponent::HUDType::POINTS);

	Entity lives = registry->createEntity(gui);
	std::string assetid = "hearts";
	SDL_Rect spriteRect = Helper::getTextureSize(assetStore, assetid);
	lives.addComponent<HUDComponent>("hearts", glm::vec2(0.0f, 0.0f), glm::vec2(spriteRect.w, spriteRect.h), HUDComponent::HUDType::HEALTH);
	
}

void Game::addSystems() {

	registry->addSystem<MovementSystem>();
	registry->addSystem<RenderSystem>();
	registry->addSystem<AnimationSystem>();
	registry->addSystem<BoxColliderSystem>();
	registry->addSystem<DebugBoxCollisionRenderer>();
	registry->addSystem<DamageSystem>();
	registry->addSystem<KeyboardSystem>();
	registry->addSystem<AISystem>();
	registry->addSystem<StaticEnemySystem>();
	registry->addSystem<ProjectileSystem>();
	registry->addSystem<ProjectilLifeTimeSystem>();
	registry->addSystem<ScrollingBackgroundRenderSystem>();
	registry->addSystem<EnemySpawnSystem>();
	registry->addSystem<EnemyBoundsCheckingSystem>();
	registry->addSystem<TextRenderSystem>();
	registry->addSystem<HealthBarRenderSystem>();
	registry->addSystem<HealthUpdateSystem>();
	registry->addSystem<ExplosionSystem>();
	registry->addSystem<DynamicTextSystem>();
	registry->addSystem<HUDRenderSystem>();
	registry->addSystem<PointSystem>();
	registry->addSystem<LivesUpdateSystem>();
	registry->addSystem<HUDLifeUpdateSystem>();
}

void Game::addTextures() {

	assetStore->addTexture(renderer, "player", "assets/images/playerShip2.png");
	assetStore->addTexture(renderer, "enemyBlack", "assets/images/enemy_black.png");
	assetStore->addTexture(renderer, "space", "assets/background/space_2.png");
	assetStore->addTexture(renderer, "playerLaser", "assets/images/player_laser.png");
	assetStore->addTexture(renderer, "engineFire", "assets/images/engine_fire.png");
	assetStore->addTexture(renderer, "enemyExplosion", "assets/images/enemyexplosion.png");
	assetStore->addTexture(renderer, "playerExplosion", "assets/images/playerexplosion.png");
	assetStore->addTexture(renderer, "enemyLaser", "assets/images/enemyLaser.png");
	assetStore->addTexture(renderer, "hearts", "assets/images/hearts.png");
}

void Game::addFonts() {
	assetStore->addFont("digiBody", "assets/fonts/DS-DIGI.TTF", 12);
	assetStore->addFont("digiBold", "assets/fonts/DS-DIGIB.TTF", 32);
}

void Game::setup() {
	loadLevel(1);
	setupEventSubscriptions();
};

void Game::setupEventSubscriptions() {

	auto& damageSystem = registry->getSystem<DamageSystem>();
	damageSystem.subscribeToEvent(eventBus);

	auto& keyboardSystem = registry->getSystem<KeyboardSystem>();
	keyboardSystem.subscribeToEvent(eventBus);

	auto& projectileSystem = registry->getSystem<ProjectileSystem>();
	projectileSystem.subscribeToEvent(eventBus);

	auto& enemySpawnSystem = registry->getSystem<EnemySpawnSystem>();
	enemySpawnSystem.subscribeToEvent(eventBus);

	auto& explosionSystem = registry->getSystem<ExplosionSystem>();
	explosionSystem.subscribeToEvent(eventBus);

	auto& healthUpdateSystem = registry->getSystem<HealthUpdateSystem>();
	healthUpdateSystem.subscribeToEvent(eventBus);

	auto& dynamicTextSystem = registry->getSystem<DynamicTextSystem>();
	dynamicTextSystem.subscribeToEvent(eventBus);

	auto& pointSystem = registry->getSystem<PointSystem>();
	pointSystem.subscribeToEvent(eventBus);

	auto& livesUpdateSystem = registry->getSystem<LivesUpdateSystem>();
	livesUpdateSystem.subscribeToEvent(eventBus);
}

void Game::run()
{
	msPreviousFrame = SDL_GetTicks();

	while (isRunning)
	{
		int start = SDL_GetTicks();

		update(deltaTime);
		processInput();
		render();

		int end = SDL_GetTicks();

		int elapsedMS = end - start;

		int delayTime = MILLISECS_PER_FRAME - elapsedMS;

		if (delayTime > 0) {
			SDL_Delay(delayTime);
		}

		deltaTime = (SDL_GetTicks() - msPreviousFrame) / 1000.0f;

		msPreviousFrame = SDL_GetTicks();
	}
};

void Game::processInput() {

	SDL_Event event;

	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
			isRunning = false;
			break;
		case SDL_KEYDOWN:
			eventBus->publishEvent<KeyboardEvent>(event.key.keysym.sym);

			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				isRunning = false;
			}
			if (event.key.keysym.sym == SDLK_l)
			{
				isDebug = !isDebug;
			}
			if (event.key.keysym.sym == SDLK_SPACE)
			{
				eventBus->publishEvent<ProjectileEvent>(registry, event.key.keysym.sym);
			}
			break;
		}
	}
}


void Game::update(float deltaTime) {
	registry->update();
	registry->getSystem<MovementSystem>().update(deltaTime);
	registry->getSystem<AISystem>().update(deltaTime);
	registry->getSystem<StaticEnemySystem>().update(eventBus, registry, 0);
	registry->getSystem<AnimationSystem>().animate();
	registry->getSystem<ProjectilLifeTimeSystem>().update();
	registry->getSystem<BoxColliderSystem>().update(eventBus, registry);
	registry->getSystem<EnemySpawnSystem>().update(registry, eventBus, assetStore, Game::mapWidth, Game::mapHeight);
	registry->getSystem<EnemyBoundsCheckingSystem>().update();
	registry->getSystem<DynamicTextSystem>().update();
	registry->getSystem<PointSystem>().update();
	registry->getSystem<HUDLifeUpdateSystem>().update(registry);
	
};

void Game::render() {

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	registry->getSystem<ScrollingBackgroundRenderSystem>().update(renderer, assetStore, deltaTime, Game::mapOffset);

	registry->getSystem<RenderSystem>().update(renderer, assetStore, Game::mapOffset);

	if (isDebug) {
		registry->getSystem<DebugBoxCollisionRenderer>().update(renderer, Game::mapOffset);
	}

	registry->getSystem<HealthBarRenderSystem>().update(renderer, Game::mapOffset);

	registry->getSystem<TextRenderSystem>().update(renderer, assetStore, Game::mapOffset);

	registry->getSystem<HUDRenderSystem>().update(assetStore, renderer);
	
	SDL_RenderPresent(renderer);
};

void Game::destroy()
{
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
};
