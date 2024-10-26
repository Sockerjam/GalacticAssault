#pragma once

#include <glm/glm.hpp>
#include "../Components/Components.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include "../Assets/AssetStore.h"

class Helper {

public:

	Helper() = default;
	~Helper() = default;

	static glm::vec2 calculcatePosition(const TransformComponent& transformComponent, const SpriteComponent& spriteComponent) {

		double radians = glm::radians(std::ceil(transformComponent.rotation));
		
		// Center Position in World Space
		glm::vec2 centerPosition = transformComponent.position + (spriteComponent.size * 0.5f);
		
		// Offset in local space
		glm::vec2 offset(spriteComponent.size.x * 0.5f, 0.0f);
		
		// Point in World Space
		glm::vec4 point(centerPosition.x + offset.x, centerPosition.y, 0.0f, 1.0f);
				
		glm::mat4 identityMatrix = glm::mat4(1.0f);

		glm::mat4 T_neg_center = glm::translate(identityMatrix, -glm::vec3(centerPosition, 0.0f));
		glm::mat4 T_center = glm::translate(identityMatrix, glm::vec3(centerPosition, 0.0f));

		glm::mat4 R = glm::rotate(identityMatrix, static_cast<float>(radians), glm::vec3(0.0f, 0.0f, 1.0f));

		glm::mat4 transformation = T_center * R * T_neg_center;

		// Apply transformation
		glm::vec4 result = transformation * point;

		// Get the new projectile position
		return glm::vec2(result.x, result.y);
	}

	static SDL_Rect getTextureSize(std::unique_ptr<AssetStore>& assetStore, std::string& assetID) {

		const auto& texture = assetStore->getTexture(assetID);

		SDL_Rect srcRect{0, 0};

		if (srcRect.w == 0 && srcRect.h == 0) {
			if (SDL_QueryTexture(texture, NULL, NULL, &srcRect.w, &srcRect.h) != 0) {
				Logger::LogErr(SDL_GetError());
				
			}	
		}

		return srcRect;

	}

	static void centerText(std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, TextLabelComponent& textLabelComponent) {

		TTF_Font* font = assetStore->getFont(textLabelComponent.assetid);

		SDL_Surface* surface = TTF_RenderText_Blended(
			font, 
			textLabelComponent.text.c_str(), 
			textLabelComponent.textColor);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);	
	
		if (texture == NULL) {
			Logger::LogErr(SDL_GetError());
		}

		SDL_FreeSurface(surface);

		SDL_Rect rect{};

		if (SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h) != 0) {
			Logger::LogErr(SDL_GetError());
		}

		textLabelComponent.position.x -= rect.w * 0.5f;
	}
	
	static void alignRight(std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer, TextLabelComponent& textLabelComponent) {

		TTF_Font* font = assetStore->getFont(textLabelComponent.assetid);

		SDL_Surface* surface = TTF_RenderText_Blended(
			font, 
			textLabelComponent.text.c_str(), 
			textLabelComponent.textColor);

		SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);	
			if (texture == NULL) {
				Logger::LogErr(SDL_GetError());
			}

		SDL_FreeSurface(surface);

		SDL_Rect rect{};

		if (SDL_QueryTexture(texture, NULL, NULL, &rect.w, &rect.h) != 0) {
			Logger::LogErr(SDL_GetError());
		}

		textLabelComponent.position.x -= rect.w;
	}
};