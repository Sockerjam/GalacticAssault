#pragma once
#include "../ECS/ESC.h"
#include "../Components/Components.h"
#include "../Helpers/Colours.h"
#include "../Assets/AssetStore.h"

class DebugBoxCollisionRenderer : public System {

public:

	DebugBoxCollisionRenderer() {
		requireComponent<TransformComponent>();
		requireComponent<BoxColliderComponent>();
	}

	void update(SDL_Renderer* renderer, int offset) {

		for (const auto& entity : getEntities()) {
			const auto& transformComponent = entity.getComponent<TransformComponent>();
			const auto& boxComponent = entity.getComponent<BoxColliderComponent>();

			SDL_Rect rect = {
				static_cast<int>(transformComponent.position.x) + static_cast<int>(boxComponent.offset.x),
				static_cast<int>(transformComponent.position.y) + static_cast<int>(boxComponent.offset.y) + offset,
				boxComponent.width * static_cast<int>(transformComponent.scale.x),
				boxComponent.height * static_cast<int>(transformComponent.scale.y)
			};

			SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
			SDL_RenderDrawRect(renderer, &rect);
		}
	}
};

class HUDRenderSystem : public System {

public:

	HUDRenderSystem() {
		requireComponent<HUDComponent>();
	}

	void update(std::unique_ptr<AssetStore>& assetStore, SDL_Renderer* renderer) {

		for (auto& entity : getEntities()) {
			
			const auto& hudComponent = entity.getComponent<HUDComponent>();

			const auto& textLabelComponent = hudComponent.textLabelComponent;

			if (textLabelComponent != nullptr) {
							
				TTF_Font* font = assetStore->getFont(textLabelComponent->assetid);

				SDL_Surface* surface = TTF_RenderText_Blended(
					font, 
					textLabelComponent->text.c_str(), 
					textLabelComponent->textColor
					);

				SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);	
					if (texture == NULL) {
						Logger::LogErr(SDL_GetError());
					}

				SDL_FreeSurface(surface);

				int labelWidth = 0;
				int labelHeight = 0;

				if (SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight) != 0) {
					Logger::LogErr(SDL_GetError());
				}

				SDL_Rect dstRect = {
					static_cast<int>(textLabelComponent->position.x),
					static_cast<int>(textLabelComponent->position.y),
					labelWidth,
					labelHeight	
				};

				SDL_RenderCopy(renderer, texture, NULL, &dstRect);
				SDL_DestroyTexture(texture);
				texture = nullptr;

			}
			else {
				
				SDL_Texture* texture = assetStore->getTexture(hudComponent.assetid);

				SDL_Rect srcRect = {
					0,
					0,
					static_cast<int>(hudComponent.size.x),
					static_cast<int>(hudComponent.size.y)
				};
			
				SDL_FRect dstRect = {
					hudComponent.position.x,
					hudComponent.position.y,
					hudComponent.size.x,
					hudComponent.size.y
				};

				SDL_RenderCopyExF(
					renderer,
					texture,
					&srcRect,
					&dstRect,
					0,
					NULL,
					SDL_FLIP_NONE);
				
			}
		}
	}

};

class TextRenderSystem : public System {

public:

	TextRenderSystem() {
		requireComponent<TextLabelComponent>();
	}

	void update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, int offset) {

		for (auto& entity : getEntities()) {

			const auto& textLabelComponent = entity.getComponent<TextLabelComponent>();
			
			TTF_Font* font = assetStore->getFont(textLabelComponent.assetid);

			SDL_Surface* surface = TTF_RenderText_Blended(
				font, 
				textLabelComponent.text.c_str(), 
				textLabelComponent.textColor
				);

			SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);	
			if (texture == NULL) {
				Logger::LogErr(SDL_GetError());
			}

			SDL_FreeSurface(surface);

			int labelWidth = 0;
			int labelHeight = 0;

			if (SDL_QueryTexture(texture, NULL, NULL, &labelWidth, &labelHeight) != 0) {
				Logger::LogErr(SDL_GetError());
			}

			SDL_Rect dstRect = {
				static_cast<int>(textLabelComponent.position.x),
				static_cast<int>(textLabelComponent.position.y) + offset,
				labelWidth,
				labelHeight	
			};

			SDL_RenderCopy(renderer, texture, NULL, &dstRect);
			SDL_DestroyTexture(texture);
			texture = nullptr;

		}
	}
};

class HealthBarRenderSystem : public System {

public:

	HealthBarRenderSystem() {
		requireComponent<HealthComponent>();
		requireComponent<TransformComponent>();
		requireComponent<SpriteComponent>();
	}

	void update(SDL_Renderer* renderer, int offset) {

		for (const auto& entity : getEntities()) {

			const auto& healthComponent = entity.getComponent<HealthComponent>();
			const auto& transformComponent = entity.getComponent<TransformComponent>();
			const auto& spriteComponent = entity.getComponent<SpriteComponent>();

			SDL_Rect rect = {
				static_cast<int>(transformComponent.position.x),
				static_cast<int>(transformComponent.position.y) + spriteComponent.srcRect.h + offset,
				static_cast<int>(static_cast<float>(spriteComponent.srcRect.w) * healthComponent.health),
				3
			};

			SDL_Color color = Color::GREEN;

			if (healthComponent.health < 0.5f) {
				color = Color::ORANGE;
			}
			
			if (healthComponent.health < 0.2) {
				color = Color::RED;
			}

			SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
			SDL_RenderFillRect(renderer, &rect);

		}
	}
};

class ScrollingBackgroundRenderSystem : public System {

private:
	
	float speed = 2.0f;

public:

	ScrollingBackgroundRenderSystem() {
		requireComponent<BackgroundComponent>();
	}

	void update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, float deltaTime, int offset) {

		float floatOffset = static_cast<float>(offset);

		for (auto& entity : getEntities()) {
			auto& backgroundComponent = entity.getComponent<BackgroundComponent>();
			const auto& texture = assetStore->getTexture(backgroundComponent.assetID);

			backgroundComponent.background.x -= deltaTime * speed;

			if (backgroundComponent.background.x <= -backgroundComponent.background.w) {

			}

			SDL_Rect srcRect{
				0,
				0,
				(int)backgroundComponent.background.w,
				(int)backgroundComponent.background.h
			};
			
			SDL_FRect dstRect{
				backgroundComponent.background.x,
				floatOffset, 
				backgroundComponent.background.w,
				backgroundComponent.background.h 
			};

			SDL_FRect dstRect2 {
				backgroundComponent.background.x + backgroundComponent.background.w,
				floatOffset,
				backgroundComponent.background.w,
				backgroundComponent.background.h 
			};

			backgroundComponent.background.x -= deltaTime * speed;

			if (backgroundComponent.background.x <= -backgroundComponent.background.w) {
				backgroundComponent.background.x += backgroundComponent.background.w;
			}

			SDL_RenderCopyExF(
				renderer,
				texture,
				&srcRect,
				&dstRect,
				NULL,
				NULL,
				SDL_FLIP_NONE);

			SDL_RenderCopyExF(
				renderer,
				texture,
				&srcRect,
				&dstRect2,
				0,
				NULL,
				SDL_FLIP_NONE);
		}
	}
};

class RenderSystem : public System {

	public:

		RenderSystem() {
			requireComponent<SpriteComponent>();
			requireComponent<TransformComponent>();
		}

		void update(SDL_Renderer* renderer, std::unique_ptr<AssetStore>& assetStore, int offset) {

			std::vector enteties = getEntities();

			for (auto& entity : enteties) {

				const auto& transformComponent = entity.getComponent<TransformComponent>();
				auto& spriteComponent = entity.getComponent<SpriteComponent>();

				const auto& texture = assetStore->getTexture(spriteComponent.assetid);

				// Set the source rectangele of our original sprite texture
				SDL_Rect srcRect = spriteComponent.srcRect;

				if (srcRect.w == 0 && srcRect.h == 0) {
					if (SDL_QueryTexture(texture, NULL, NULL, &srcRect.w, &srcRect.h) != 0) {
						Logger::LogErr(SDL_GetError());
					}
				}

				if (spriteComponent.size == glm::vec2(0, 0)) {
					spriteComponent.size = glm::vec2(srcRect.w, srcRect.h);
				}

				// Set the destination rectangle with the x and y position to be rendered
				SDL_FRect dstRect = {
					transformComponent.position.x,
					transformComponent.position.y + offset,
					spriteComponent.size.x * transformComponent.scale.x,
					spriteComponent.size.y * transformComponent.scale.x
				};

				SDL_RenderCopyExF(
					renderer,
					texture,
					&srcRect,
					&dstRect,
					transformComponent.rotation,
					NULL,
					SDL_FLIP_NONE);
			}
		}	
};
