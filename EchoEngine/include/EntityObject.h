#pragma once
#include <string>
#include <vector>
#include <DDECS.h>
#include <Name.h>

/*namespace EntityObject {
	struct ComponentObject {
		std::string name;
		void* data;
		enum {
			TYPE_INT,
			TYPE_STRING,
			TYPE_ARRAY_INT,
			TYPE_OBJECT
		} type;
	};
}*/

struct ComponentObject {
	void* data = NULL;
	std::string name = "";
	void destruct() {
		size = 0;
		//name = "";
		if (type == TYPE::TYPE_ARRAY)
			free(data);
	}
	enum class TYPE : uint8_t {
		TYPE_NULL,
		TYPE_INT,
		TYPE_STRING,
		TYPE_ARRAY//,
		//TYPE_OBJECT
	} type = TYPE::TYPE_NULL;
	size_t size = 0;
	void setNULL() {
		destruct();
		type = TYPE::TYPE_NULL;
	}
	void setInt(uint32_t value) {
		destruct();
		type = TYPE::TYPE_INT;
		size = sizeof(uint32_t);
		memcpy(&data, &value, sizeof(uint32_t));
	}
	void setString(const char* value) {
		/*destruct();
		type = TYPE::TYPE_STRING;
		size = strlen(value);
		if (size > 256) throw;
		data = malloc(size*2+2);
		if (data == NULL) throw;
		for (size_t i = 0; i < size; i++)
			((char*)data)[i] = value[i];
		#pragma warning(suppress: 6386)
		((char*)data)[size] = 0;*/
		destruct();
		type = TYPE::TYPE_STRING;
		size = sizeof(Name);
		Name nameValue = value;
		static_assert(sizeof(Name) <= sizeof(void*), "Size of Name is greater than size of void*");
		memcpy(&data, &nameValue, sizeof(Name));
	}
	void setArray(const std::vector<uint32_t> value) {
		destruct();
		type = TYPE::TYPE_ARRAY;
		size = value.size() * sizeof(uint32_t);
		data = malloc(size);
		if (data == NULL) throw;
		memcpy(data, value.data(), size);
	}

	uint32_t getInt() {
		return (uint32_t)(intptr_t)data;
	}
	Name getString() {
		Name retValue = "";
		memcpy(&retValue, &data, sizeof(Name));
		return retValue;
	}
	uint32_t* getArray() {
		return (uint32_t*)data;
	}
	uint32_t getArrayIntLen() {
		return size / sizeof(uint32_t);
	}
};
struct EntityObject {
	std::vector<ComponentObject> components;
	ComponentObject* getComponent(const std::string name) {
		for (ComponentObject& c : components)
			if (c.name == name)
				return &c;
		return nullptr;
	}
	void* getComponentData(const std::string);
};

