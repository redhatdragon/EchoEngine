#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include "FlatBuffer.h"

typedef uint32_t EntityID;
typedef uint32_t ComponentID;

template<uint32_t max_components, uint32_t max_entities>
class DDECS {
	struct Entity {
		FlatFlaggedBuffer<uint32_t, max_components> componentIndexes;
		void clear() {
			memset(this, 0, sizeof(*this));
		}
	};
	FlatFlaggedBuffer<Entity, max_entities> entities;
	class ComponentBuffer {
	public:
		uint8_t* data;
		uint32_t count = 0;
		EntityID owner[max_entities];
		uint32_t componentSize;
		std::string name;
		inline void init(const std::string& componentName, uint32_t size) {
			data = (uint8_t*)malloc(size * max_entities);
			componentSize = size;
			name = componentName;
		}
		inline uint32_t getCount() { return count; }
		inline EntityID getOwner(uint32_t index) { return owner[index]; }
		inline void* getData() { return (void*)data; }
		inline const std::string& getName() { return name; }
		inline uint32_t getComponentSize() { return componentSize; }
		inline void push(void* _data, EntityID entityID) {
			memcpy(&data[count * componentSize], _data, componentSize);
			owner[count] = entityID;
			count++;
		}
		inline EntityID remove(uint32_t index) {
			count--;
			memcpy(&data[index * componentSize], &data[count * componentSize], componentSize);
			owner[index] = owner[count];
			//TODO: This needs rework not sure how though.
			EntityID retValue = owner[count];
			owner[count] = -1;  //TODO: should be safe to remove without any consequence.
			//return owner[count];
			return retValue;
		}
	};
	FlatBuffer<ComponentBuffer, max_components> components;
public:
	DDECS() {
		for (unsigned int i = 0; i < max_entities; i++) {
			entities[i].clear();
		}
	}
	EntityID getNewEntity() {
		entities.incrementCount();
		return entities.toggleFirstInvalid();
	}
	void removeEntity(EntityID entity) {
		auto &compIndexes = entities[entity].componentIndexes;
		uint32_t compCount = compIndexes.getCount();
		uint32_t i = 0, foundCount = 0;
		while (foundCount < compCount) {
			if (compIndexes.getIsValid(i)) {
				removeComponent(entity, i);
				foundCount++;
			}
			i++;
		}
		entities[entity].clear();
		entities.decrementCount();
		entities.setInvalid(entity);
	}
	ComponentID registerComponentUnsafe(const std::string& componentName, uint32_t size) {
		ComponentID retValue = { components.count };
		ComponentBuffer *buffer = &components[components.count];
		buffer->init(componentName, size);
		components.count++;
		return retValue;
	}
	ComponentID registerComponent(const std::string& componentName, uint32_t size) {
		ComponentID retValue = getComponentID(componentName);
		if (retValue == -1)
			return registerComponentUnsafe(componentName, size);
		return retValue;
	}
	ComponentID getComponentID(const std::string& componentName) {
		auto count = components.count;
		for (uint32_t i = 0; i < count; i++) {
			if (components[i].getName() == componentName)
				return { i };
		}
		return { (uint32_t)-1 };
	}
	void emplace(EntityID entity, ComponentID componentID, void* data) {
		ComponentBuffer* buffer = &components[componentID];
		buffer->push(data, entity);
		auto componentSize = buffer->getComponentSize();
		entities[entity].componentIndexes[componentID] = buffer->getCount()-1;
		entities[entity].componentIndexes.setValid(componentID);
		entities[entity].componentIndexes.incrementCount();
	}
	void removeComponent(EntityID entity, ComponentID componentID) {
		auto index = entities[entity].componentIndexes[componentID];
		EntityID otherEntity = components[componentID].remove(index);
		entities[entity].componentIndexes.setInvalid(componentID);
		entities[entity].componentIndexes.decrementCount();
		entities[otherEntity].componentIndexes[componentID] = index;
	}
	void* getComponentBuffer(ComponentID componentID) {
		return components[componentID].getData();
	}
	uint32_t getComponentCount(ComponentID componentID) {
		return components[componentID].getCount();
	}
	EntityID getOwner(ComponentID id, uint32_t index) {
		return components[id].getOwner(index);
	}
	void* getEntityComponent(EntityID entity, ComponentID componentID) {
		uint32_t componentIndex = entities[entity].componentIndexes[componentID];
		if (entities[entity].componentIndexes.getIsValid(componentID) == false)
			return nullptr;
		ComponentBuffer* buffer = &components[componentID];
		uint8_t* data = (uint8_t*)buffer->getData();
		data += componentIndex * buffer->getComponentSize();
		return (void*)data;
	}
	template<typename T>
	T getEntityComponentAs(EntityID entity, ComponentID componentID) {
		void* retValue = getEntityComponent(entity, componentID);
		return *(T*)retValue;
	}
	bool entityHasComponent(EntityID entity, ComponentID componentID) {
		if (getEntityComponent(entity, componentID)) return true;
		return false;
	}
	uint32_t getEntityCount() {
		return entities.getCount();
	}
};