#pragma once
#include <string>
#include <vector>
#include <DDECS.h>
#include <Name.h>
#include <FixedPoint.h>
#include <DArray.h>

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
		TYPE_FIXED_FLOAT,
		TYPE_STRING,
		TYPE_ARRAY,
		TYPE_DARRAY//,
		//TYPE_OBJECT
	} type = TYPE::TYPE_NULL;
	size_t size = 0;
	const char* typeAsString() {
		switch (type) {
		case TYPE::TYPE_NULL:
			return "NULL";
		case TYPE::TYPE_INT:
			return "INT";
		case TYPE::TYPE_FIXED_FLOAT:
			return "FIXED_FLOAT";
		case TYPE::TYPE_STRING:
			return "STRING";
		case TYPE::TYPE_ARRAY:
			return "ARRAY";
		case TYPE::TYPE_DARRAY:
			return "DARRAY";
		default:
			return "INVALID!";
		}
	}
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
	void setFixedPoint(FixedPoint<> value) {
		destruct();
		type = TYPE::TYPE_FIXED_FLOAT;
		size = sizeof(FixedPoint<>);
		memcpy(&data, &value, sizeof(FixedPoint<>));
	}
	void setString(const char* value) {
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
	void setDArray(DArray<uint32_t> value) {
		destruct();
		type = TYPE::TYPE_DARRAY;
		size = sizeof(value);
		data = malloc(size);
		if (data == NULL) throw;
		memcpy(data, &value, size);
	}

	uint32_t getInt() {
		return (uint32_t)(intptr_t)data;
	}
	FixedPoint<> getFixedPoint() {
		FixedPoint<> retValue;
		retValue.setRaw((int32_t)(intptr_t)data);
		return retValue;
	}
	Name getString() {
		Name retValue = "";
		memcpy(&retValue, &data, sizeof(Name));
		return retValue;
	}
	uint32_t* getArray() {
		return (uint32_t*)data;
	}
	DArray<uint32_t> getDArray() {
		DArray<uint32_t> retValue;
		memcpy(&retValue, &data, sizeof(DArray<uint32_t>));
		return retValue;
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

