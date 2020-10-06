#pragma once
#include "IO_API/IO_API.h"
#include <unordered_map>
#include <iostream>

struct TextureID { uint32_t id; };
struct SoundID { uint32_t id; };

namespace TextureCodex {
	static constexpr uint32_t max = 256;
	extern void* textures[max];
	extern std::unordered_map<std::string, uint32_t> hashes;
	extern uint32_t refCount[max];
	extern uint32_t textureCount;

	static TextureID add(const std::string& path) {
		const auto& element = hashes.find(path);
		if (element != hashes.end()) {
			refCount[element->second]++;
			return { element->second };
		}
		textureCount++;
		hashes[path] = textureCount;
		void* newTexture = getTexture(path.c_str());
		textures[textureCount] = newTexture;
		std::cout << "Caching new texture: " << textureCount << " " << path << std::endl;
		return { textureCount };
	}

	static void* get(TextureID id) {
		return textures[id.id];
	}
};