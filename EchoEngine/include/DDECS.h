#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <FlatBuffer.h>

typedef uint32_t EntityID;
typedef uint32_t ComponentID;

template<uint32_t max_components, uint32_t max_entities>
class DDECS {
	struct Entity {
		FlatFlaggedBuffer<uint32_t, max_components> componentIndexes;
		void clear() {
			memset(this, 0, sizeof(*this));  //Might not be working.
			/*auto count = componentIndexes.getCount();
			for(auto i = 0, found = 0; found < count; i++) {
				if (componentIndexes.getIsValid(i));
			}*/
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
			return owner[count];
			owner[count] = -1;
		}
	};
	FlatBuffer<ComponentBuffer, max_components> components;
public:
	DDECS() {
		/*for (unsigned int i = 0; i < max_entities; i++) {
			for (unsigned int j = 0; j < max_components; j++) {
				entities[i].componentIndexes[j] = -1;
			}
		}*/
		for (unsigned int i = 0; i < max_entities; i++) {
			entities[i].clear();
		}
	}
	EntityID getNewEntity() {
		entities.incrementCount();
		//entities[entities.getCount() - 1].clear();
		return entities.toggleFirstInvalid();
	}
	void removeEntity(EntityID entity) {
		auto &compIndexes = entities[entity].componentIndexes;
		uint32_t compCount = compIndexes.getCount();
		uint32_t i = 0, foundCount = 0;
		while (foundCount < compCount) {
			if (compIndexes.getIsValid(i)) {
				//auto index = compIndexes[i];
				//Entity& otherEntity = entities[components[i].remove(index)];
				removeComponent(entity, i);

				//compIndexes.setInvalid(i);
				foundCount++;
			}
			i++;
		}
		//compIndexes.clear();
		entities[entity].clear();
		entities.decrementCount();
		entities.setInvalid(entity);
		//memcpy(&entities[entity], &entities[entities.getCount()], sizeof(entities[entity]));
		//memcpy(&entities[entity], &entities[entities.getCount()], sizeof(Entity));
		//entities[entity] = entities[entities.getCount()];
	}
	ComponentID registerComponent(const std::string& componentName, uint32_t size) {
		ComponentID retValue = { components.count };
		ComponentBuffer *buffer = &components[components.count];
		buffer->init(componentName, size);
		components.count++;
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
	/*ComponentID getOrRegisterComponentID(const std::string& componentName) {
		ComponentID retValue = getComponentID(componentName);
		if (retValue == -1) 
			return registerComponent()
	}*/
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
		//TODO: Weird complicated bit should rethink probably.
		//entities[otherEntity].componentIndexes[componentID] = components[componentID].getCount() - 1;
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
		//uint8_t* data = &components[componentID].data[componentIndex*components[componentID].owner.count];
		ComponentBuffer* buffer = &components[componentID];
		uint8_t* data = (uint8_t*)buffer->getData();
		//data += componentIndex*buffer->getCount();
		data += componentIndex * buffer->getComponentSize();
		return (void*)data;
	}
	uint32_t getEntityCount() {
		return entities.getCount();
	}
};