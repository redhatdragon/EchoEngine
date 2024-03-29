#pragma once
#include <unordered_map>

class Name {
	static std::unordered_map<std::string, uint32_t> map;
	static std::vector<std::string> strings;
	uint32_t id = 0;
public:
	Name() = default;
	Name(std::string& str) {
		if (map.find(str) == map.end()) {
			uint32_t newID = (uint32_t)strings.size();
			map[str] = newID;
			strings.push_back(str);
			id = newID;
			return;
		}
		id = map[str];
	}
	Name(const char* str) {
		if (map.find(str) == map.end()) {
			uint32_t newID = (uint32_t)strings.size();
			map[str] = newID;
			strings.push_back(str);
			id = newID;
			return;
		}
		id = map[str];
	}
	inline const char* c_str() {
		return strings[id].c_str();
	}
	inline uint32_t getID() {
		return id;
	}

	bool operator==(Name other) {
		if (id == other.id)
			return true;
		return false;
	}
	bool operator==(std::string& otherStr) {
		Name other = otherStr;
		if (id == other.id)
			return true;
		return false;
	}
	bool operator==(const char* otherStr) {
		Name other = otherStr;
		if (id == other.id)
			return true;
		return false;
	}
};