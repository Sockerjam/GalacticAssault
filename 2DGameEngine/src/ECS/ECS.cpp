#include "ESC.h"

int IComponent::nextID = 0;

int Entity::getID() const {
	return id;
}

void System::addEntity(Entity entity) {
	enteties.push_back(entity);
};

void System::removeEntity(Entity entity) {
	enteties.erase(std::remove_if(enteties.begin(), enteties.end(), [&entity](Entity other) {
		return entity == other;
		}), enteties.end());
};

void System::sortEnteties() {
	std::sort(enteties.begin(), enteties.end(), sortByLayer);
	Logger::Log("System Enteties Sorted");
}

std::vector<Entity> System::getEntities() const {
	return enteties;
};

const Signature& System::getCompontentSignature() const {
	return componentSignature;
};

Entity Registry::createEntity(Layer layer) {
	int entityID;

	if (freeIDs.empty()) {
		entityID = numEntities++;
		if (entityID >= static_cast<int>(entityComponentSignatures.size())) {
			entityComponentSignatures.resize(entityID + 1);
		}
	}
	else {
		entityID = freeIDs.front();
		freeIDs.pop_front();
	}

	Entity entity(entityID, layer);
	entity.registry = this;

	entitiesToBeAdded.insert(entity);

	return entity;

}

void Registry::killEntity(Entity entity) {
	entitiesToBeRemoved.insert(entity);
}

void Registry::setPlayerEntity(Entity& entity) {
	playerEntity = std::make_unique<Entity>(entity);
}

std::unique_ptr<Entity>& Registry::getPlayerEntity() {
	return playerEntity;
}

Layer Entity::getLayer() const {
	return layer;
}

void Entity::kill() {
	registry->killEntity(*this);
}

void Registry::addEntityToSystem(const Entity& entity) {
	const auto& entityid = entity.getID();

	const auto& entityComponentSignature = entityComponentSignatures[entityid];

	for (auto& system : systems) {

		const auto& systemComponentSignature = system.second->getCompontentSignature();

		bool isInterested = (entityComponentSignature & systemComponentSignature) == systemComponentSignature;

		if (isInterested) {
			system.second->addEntity(entity);
		}
	}
}

void Registry::removeEntityFromSystems(const Entity& entity) {

	for (auto system : systems) {
		system.second->removeEntity(entity);
	}

}

void Registry::sortSystemEntetiesByLayers() {
	for (const auto& system : systems) {
		system.second->sortEnteties();
		Logger::Log("Enteties Sorted: " + std::to_string(system.second->getEntities().size()));
	}
}

void Registry::update() {
	// Add the entites that are waiting to be added to a system

	for (auto& entity : entitiesToBeAdded) {
		addEntityToSystem(entity);
	}

	if (!entitiesToBeAdded.empty()) {
		sortSystemEntetiesByLayers();
	}

	entitiesToBeAdded.clear();

	// Remove entities that are waiting to be removed

	for (auto& entity : entitiesToBeRemoved) {
		removeEntityFromSystems(entity);
		
		entityComponentSignatures[entity.getID()].reset();

		freeIDs.push_back(entity.getID());

		Logger::Log("FREE ID: " + std::to_string(entity.getID()));
	}

	entitiesToBeRemoved.clear();

}

Registry::Registry() {
	Logger::Log("Registry Created");
}

Registry::~Registry() {
	Logger::Log("Registry Destroyed");
}
