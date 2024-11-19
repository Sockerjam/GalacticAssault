#pragma once
#include "../Logger/Logger.h"
#include <SDL.h>
#include "Event.h"
#include <map>
#include <list>
#include <typeindex>
#include <memory>
#include <functional>

class IEventCallback {
private:

	virtual void call(Event& e) = 0;

public:

	virtual ~IEventCallback() = default;

	void execute(Event& e) {
		call(e);
	}


};

template <typename TOwner, typename TEvent>
class EventCallback : public IEventCallback {

private:

	typedef void (TOwner::* Callback)(TEvent&);

	TOwner* owner;
	Callback callbackFunction;

	virtual void call(Event& e) override {
		std::invoke(callbackFunction, owner, static_cast<TEvent&>(e));
	}

public:

	EventCallback(TOwner* owner, Callback callbackFunction) {
		this->owner = owner;
		this->callbackFunction = callbackFunction;
	}

	virtual ~EventCallback() override = default;

};


typedef std::list<std::unique_ptr<IEventCallback>> HandlerList;

class EventBus {
private:

	std::map<std::type_index, std::unique_ptr<HandlerList>> subscribers;

public:

	EventBus() {
		Logger::Log("Event Bus Created");
	}

	~EventBus() {
		Logger::Log("Event Bus Destroyed");
	}

	template <typename TOwner, typename TEvent>
	void subscribeToEvent(TOwner* owner, void (TOwner::*Callback)(TEvent&));

	template <typename TEvent, typename ...TArgs>
	void publishEvent(TArgs&& ...args);
	
};

template <typename TOwner, typename TEvent>
void EventBus::subscribeToEvent(TOwner* owner, void (TOwner::*Callback)(TEvent&)) {

	if (!subscribers[typeid(TEvent)].get()) {
		subscribers[typeid(TEvent)] = std::make_unique<HandlerList>();
	}

	auto subscriber = std::make_unique<EventCallback<TOwner, TEvent>>(owner, Callback);
	subscribers[typeid(TEvent)].get()->push_back(std::move(subscriber));
};

template <typename TEvent, typename ...TArgs>
void EventBus::publishEvent(TArgs&& ...args) {

	auto handlers = subscribers[typeid(TEvent)].get();

	if (handlers) {

		for (auto it = handlers->begin(); it != handlers->end(); it++) {
			auto handler = it->get();
			TEvent event(std::forward<TArgs>(args)...);
			handler->execute(event);
		}
	}
}
