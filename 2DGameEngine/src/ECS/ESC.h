#pragma once
#include "../Logger/Logger.h"
#include <bitset>
#include <vector>
#include <unordered_map>
#include <typeindex>
#include <set>
#include <memory>
#include <algorithm>
#include <deque>

const unsigned int MAX_COMPONENTS = 32;

/// <summary>
/// Signature
/// We use a bitset (1 and 0) to keep track of which components an Entity has
/// and which enteties a System is interested in
/// </summary>
typedef std::bitset<MAX_COMPONENTS> Signature;

enum Layer {
	tileMap = 0,
	explosion,
	enemy,
	player,
	projectile,
	gui
};

enum EntityType {
	PLAYER,
	ENEMY
};

class Entity {

private:
	int id;
	Layer layer;
public:
	Entity(int id, Layer layer) : id(id), layer(layer) {};
	
	int getID() const;

	bool operator ==(const Entity& other) const {
		return id == other.id;
	};

	bool operator <(const Entity& other) const {
		return id < other.id;
	};

	Layer getLayer() const;
	void kill();

	template <typename TComponent, typename ...TArgs>
	void addComponent(TArgs&& ...args);

	template <typename TComponent>
	void removeComponent();

	template <typename TComponent>
	bool hasComponent() const;

	template <typename TComponent>
	TComponent& getComponent() const;

	// Forward decleration of the class Registry
	// which is defined later.
	// This is so the compiler can see a decleration of Registry
	class Registry* registry;
};

struct IComponent {
protected:
	static int nextID;
};

template <typename T>
class Component: public IComponent {

public:
	static int getID() {
		static auto id = nextID++;
		return id;
	}
};

/// <summar>
/// The system processes enteties that contain a specific component
/// </summary>
class System {

private:

	Signature componentSignature;
	std::vector<Entity> enteties;

	static bool sortByLayer(const Entity& entity1, const Entity& entity2) {
		return entity1.getLayer() < entity2.getLayer();
	};

public:

	System() = default;
	~System() = default;

	void addEntity(const Entity entity);
	void sortEnteties();
	void removeEntity(Entity entity);

	std::vector<Entity> getEntities() const;
	const Signature& getCompontentSignature() const;

	template <typename TComponent> 
	void requireComponent();
};

class IPool {
public:
	virtual ~IPool() {};
};

template <typename T>
class Pool: public IPool {
private:
	std::vector<T> data;

public:

	Pool() {
	}

	virtual ~Pool() = default;

	bool isEmpty() {
		return data.empty();
	}

	int getSize() {
		return data.size();
	}

	void resize(int size) {
		data.resize(size);
	}

	void clear() {
		data.clear();
	}

	void addObject(T object) {
		data.push_back(object);
	}

	void set(int index, T object) {
		data[index] = object;
	}

	T& getObject(int index) {
		return static_cast<T&>(data[index]);
	}

	T& operator [](unsigned int index) {
		return data[index];
	}
};

/// <summary>
/// The registry manages the creation and destruction of Entities,
/// as well as adding systems and adding components to entities
/// </summary>
class Registry {
	
private:

	int numEntities = 0;

	std::unique_ptr<Entity> playerEntity;

	// A vector of components pools, each index is a component type which has a pool of
	// entities that has that component. For example, tranform component
	std::vector<std::shared_ptr<IPool>> componentPools;

	// Vector of component signatures per entity
	// Vector index = entity id
	std::vector<Signature> entityComponentSignatures;

	std::unordered_map<std::type_index, std::shared_ptr<System>> systems;

	std::set<Entity> entitiesToBeAdded;
	std::set<Entity> entitiesToBeRemoved;

	std::deque<int> freeIDs;

public:

	Registry();

	~Registry();

	Entity createEntity(Layer layer);
	void killEntity(Entity entity);
	void setPlayerEntity(Entity& entity);
	std::unique_ptr<Entity>& getPlayerEntity();

	void sortSystemEntetiesByLayers();

	template <typename TComponent, typename ...TArgs>
	void addComponent(const Entity& entity, TArgs&& ...args);

	template <typename TComponent>
	void removeComponent(const Entity& entity);

	template <typename TComponent>
	bool hasComponent(const Entity& entity) const;

	template <typename TComponent>
	TComponent& getComponent(const Entity& entity) const;

	void update();

	//Systems

	template <typename TSystem, typename ...TArgs>
	void addSystem(TArgs&& ...args);

	template <typename TSystem>
	void removeSystem();

	template <typename TSystem>
	bool hasSystem() const;

	template <typename TSystem>
	TSystem& getSystem() const;

	void addEntityToSystem(const Entity& entity);
	void removeEntityFromSystems(const Entity& entity);

};

template <typename TSystem, typename ...TArgs>
void Registry::addSystem(TArgs&& ...args) {
	std::shared_ptr<TSystem> system = std::make_shared<TSystem>(TSystem(std::forward<TArgs>(args)...));
	systems.insert(std::make_pair(std::type_index(typeid(TSystem)), system));
}

template <typename TSystem>
void Registry::removeSystem() {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	systems.erase(system);
}

template <typename TSystem>
bool Registry::hasSystem() const {
	return systems.find(std::type_index(typeid(TSystem))) != systems.end();
}

template <typename TSystem>
TSystem& Registry::getSystem() const {
	auto system = systems.find(std::type_index(typeid(TSystem)));
	return *(std::static_pointer_cast<TSystem>(system->second));
}

template <typename TComponent, typename ...TArgs>
void Registry::addComponent(const Entity& entity, TArgs&& ...args) {

	const auto componentID = Component<TComponent>::getID();

	const auto entityID = entity.getID();

	// Resizing the vector to be able to store the component
	if (componentID >= static_cast<int>(componentPools.size())) {
		componentPools.resize(componentID + 1, nullptr);
	}

	// If the componentPool deosn't exist, create it and add it to the componentPool vector
	if (!componentPools[componentID]) {
		std::shared_ptr<Pool<TComponent>> newComponentPool = std::make_shared<Pool<TComponent>>();
		componentPools[componentID] = newComponentPool;
	}

	std::shared_ptr<Pool<TComponent>> componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentID]);

	// Resize the specific componentPool to fit the entity index
	if (entityID >= componentPool->getSize()) {
		componentPool->resize(numEntities);
	}

	TComponent newComponent(std::forward<TArgs>(args)...);

	componentPool->set(entityID, newComponent);

	entityComponentSignatures[entityID].set(componentID);

	Logger::Log("Component id: " + std::to_string(componentID) + " was added to entity id: " + std::to_string(entityID));
}

template <typename TComponent>
void Registry::removeComponent(const Entity& entity) {

	const auto componentID = Component<TComponent>::getID();
	const auto entityID = entity.getID();

	entityComponentSignatures[entityID].set(componentID, 0);

	Logger::Log("Component id: " + std::to_string(componentID) + " was removed from entity id: " + std::to_string(entityID));
}

template <typename TComponent>
bool Registry::hasComponent(const Entity& entity) const {

	const auto componentID = Component<TComponent>::getID();
	const auto entityID = entity.getID();

	return entityComponentSignatures[entityID].test(componentID);
}

template <typename TComponent>
TComponent& Registry::getComponent(const Entity& entity) const {
	const auto componentID = Component<TComponent>::getID();
	const auto entityID = entity.getID();
	
	const auto componentPool = std::static_pointer_cast<Pool<TComponent>>(componentPools[componentID]);
  
	return componentPool->getObject(entityID);
};

template <typename TComponent> 
void System::requireComponent() {
	const auto componentID = Component<TComponent>::getID();
	componentSignature.set(componentID);
};

template <typename TComponent, typename ...TArgs>
void Entity::addComponent(TArgs&& ...args) {
	registry->addComponent<TComponent>(*this, std::forward<TArgs>(args)...);
}

template <typename TComponent>
void Entity::removeComponent() {
	registry->removeComponent<TComponent>(*this);
}

template <typename TComponent>
bool Entity::hasComponent() const {
	return registry->hasComponent<TComponent>(*this);
}

template <typename TComponent>
TComponent& Entity::getComponent() const {
	return registry->getComponent<TComponent>(*this);
}
