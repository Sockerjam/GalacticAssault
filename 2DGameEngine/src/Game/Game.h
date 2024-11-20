#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <glm/glm.hpp>
#include "../ECS/ESC.h"
#include "../Assets/AssetStore.h"
#include "../Events/EventBus.h"

const int FPS = 120;
const int MILLISECS_PER_FRAME = 1000 / FPS;

class Game {

private:
	bool isRunning;
	int msPreviousFrame;
	float deltaTime;
	float centerX;
	float centerY;

	SDL_DisplayMode displayMode;
	SDL_Window* window;
	SDL_Renderer* renderer;

	std::unique_ptr<Registry> registry;
	std::unique_ptr<AssetStore> assetStore;
	std::unique_ptr<EventBus> eventBus;

	void setCenterValues();
	void loadLevel(int level);
	void setupEventSubscriptions();
	void addSystems();
	void addTextures();
	void addFonts();
	void addSounds();
	void createBackground();
	void createPlayer();
	void createHUDComponents();
	void playBackgroundMusic();
	void startEngineSound();

	bool isDebug;

public:
	Game();
	~Game();

	static int windowWidth;
	static int windowHeight;
	static int mapWidth;
	static int mapHeight;
	static int mapOffset;

	void run();
	void processInput();
	void update(float deltaTime);
	void render();
	void destroy();
	void setup();
};
