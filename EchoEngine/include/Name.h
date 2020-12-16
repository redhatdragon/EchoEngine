#pragma once
#include <unordered_map>

class Name {
	static std::unordered_map<std::string, uint32_t> map;
	static std::vector<std::string> strings;
	uint32_t id = 0;
public:
	Name(Name& other) {
		id = other.id;
	}
	Name(std::string& str) {
		uint32_t newID;
		if (map.find(str) == map.end()) {
			newID = strings.size();
			map[str] = newID;
			strings.push_back(str);
			id = newID;
			return;
		}
		newID = map[str];
	}
	Name(const char* str) {
		uint32_t newID;
		if (map.find(str) == map.end()) {
			newID = strings.size();
			map[str] = newID;
			strings.push_back(str);
			id = newID;
			return;
		}
		newID = map[str];
	}
	inline const char* c_str() {
		return strings[id].c_str();
	}
	inline uint32_t getID() {
		return id;
	}
};