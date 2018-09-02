#pragma once

#include <functional>
#include <unordered_map>
#include <memory>

#define DELEGATE_FROM_THIS(param, method_ref, name) Delegate< param > name ([this]( param p) { method_ref (p); });
#define DELEGATE_FROM_CLASS_REF(param, method_ref, obj_ref, name) Delegate< param > name ( [& obj_ref ]( param p ) { obj_ref . method_ref (p) }

typedef size_t DelegateID;

template<typename... TArgs>
using Delegate = std::function<void(TArgs...)>;

template<typename... TArgs>
class Event
{
public:
	Event() = default;
	DelegateID Subscribe(Delegate<TArgs...>&& func)
	{
		delegate_map.insert({ ++uid, std::move(func) });
		return uid;
	}
	DelegateID Subscribe(const Delegate<TArgs...>& func)
	{
		delegate_map.insert({ ++uid, func });
		return uid;
	}

	bool Unsubscribe(DelegateID id)
	{
		return (delegate_map.erase(id) > 0);
	}


	void Fire(TArgs... args) const
	{
		for(const std::pair<DelegateID,Delegate<TArgs...>>& pair : delegate_map)
		{
			pair.second(args...);
		}
	}
	
private:
	std::unordered_map<DelegateID, Delegate<TArgs...>> delegate_map;
	DelegateID uid = 0;
};

template<typename... TArgs>
class DelegateGuard
{
public:
	DelegateGuard() = default;

	DelegateGuard(DelegateGuard&& guard)
	{
		event = std::move(guard.event);
		id = guard.id;
		guard.id = 0;
	}

	DelegateGuard& operator=(DelegateGuard&& guard)
	{
		if (this != &guard) {
			event = std::move(guard.event);
			id = guard.id;
			guard.id = 0;
		}
		return *this;
	}

	DelegateGuard(std::shared_ptr<Event<TArgs...>>& event, Delegate<TArgs...>&& func)
	{
		id = event->Subscribe(std::move(func));
		this->event = std::weak_ptr<Event<TArgs...>>(event);
	}

	DelegateGuard(std::shared_ptr<Event<TArgs...>>& event, const Delegate<TArgs...>& func)
	{
		id = event->Subscribe(func);
		this->event = std::weak_ptr<Event<TArgs...>>(event);
	}

	~DelegateGuard()
	{
		if(!event.expired() || id != 0)
			event.lock()->Unsubscribe(id);
	}

	DelegateGuard(const DelegateGuard& guard) = delete;
	DelegateGuard& operator=(const Event<TArgs...>& guard) = delete;
	std::weak_ptr<Event<TArgs...>>& GetEvent() { return event; }

private:
	std::weak_ptr<Event<TArgs...>> event;
	DelegateID id = 0;
};

template <typename T>
using SharedEvent = std::shared_ptr<Event<T>>;

