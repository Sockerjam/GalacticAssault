#pragma once
#include "SDL.h"

struct Color {
	static constexpr SDL_Color WHITE{ 255, 255, 255 };
	static constexpr SDL_Color GREEN{ 0, 255, 0 };
	static constexpr SDL_Color RED{ 255, 0, 0 };
	static constexpr SDL_Color ORANGE{ 255, 165, 0 };
};
