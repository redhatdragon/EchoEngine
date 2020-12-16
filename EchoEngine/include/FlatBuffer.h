#pragma once

#include <stdint.h>
#include <vector>

/*template <typename T, uint32_t maxLength>
class FlatFlaggedBuffer {
	T data[maxLength];
	uint8_t isValid[maxLength / 8 + 8];
	uint32_t count = 0;
public:
	FlatFlaggedBuffer() {
		for (uint32_t i = 0; i < maxLength / 8 + 8; i++) {
			isValid[i] = 0;
		}
	}
	__forceinline uint32_t toggleFirstInvalid() {
		uint32_t c = maxLength / 4 + 1;
		uint32_t* isValidPtr = (uint32_t*)isValid;
		for (uint32_t i = 0; i < c; i++) {
			if (isValidPtr[i] != -1) {
				for (unsigned int j = 0; j < 32; j++) {
					if (isValidPtr[i] | 1 << j == 0) {
						isValidPtr[i] |= 1 << j;
						//return i * j;
						return i * 32 + j;
					}
				}
			}
		}
		return -1;
	}
	__forceinline void setInvalid(uint32_t index) {
		uint32_t i = index / 8;
		uint32_t remainder = index % 8;
		isValid[i] &= ~(1 << remainder);
	}
	__forceinline void setValid(uint32_t index) {
		uint32_t i = index / 8;
		uint32_t remainder = index % 8;
		isValid[i] |= 1 << remainder;
	}
	__forceinline uint32_t insert(const T& component) {
		uint32_t index = toggleFirstInvalid();
		count++;
		data[index] = component;
		setValid(index);
		return index;
	}
	__forceinline bool getIsValid(uint32_t index) {
		uint32_t i = index / 8;
		uint32_t remainder = index % 8;
		return isValid[i] | 1 << remainder;
	}
	__forceinline uint32_t getCount() {
		return count;
	}
	__forceinline T operator[](uint32_t index) {
		return data[index];
	}
};*/

template <typename T, uint32_t maxLength>
class FlatFlaggedBuffer {
	T data[maxLength];
	uint8_t isValid[maxLength];
	uint32_t count = 0;
	uint32_t maxIndex = 0;
public:
	FlatFlaggedBuffer() {
		clear();
	}
	__forceinline void clear() {
		for (uint32_t i = 0; i < maxLength; i++) {
			isValid[i] = 0;
		}
	}
	// Consider using in conjunction with incrementCount.
	__forceinline uint32_t toggleFirstInvalid() {
		uint32_t max = maxLength / 4;
		uint32_t* bufferAs32 = (uint32_t*)&isValid[0];
		for (unsigned int i = 0; i < max; i++) {
			if (bufferAs32[i] == -1) continue;
			for (unsigned int j = 0; j < 4; j++) {
				if (isValid[i * 4 + j] == 0) {
					setValid(i * 4 + j);
					return i * 4 + j;
				}
			}
		}
		return -1;
	}
	__forceinline void setInvalid(uint32_t index) {
		isValid[index] = 0;
	}
	__forceinline void setValid(uint32_t index) {
		isValid[index] = 255;
	}
	__forceinline uint32_t insert(const T& component) {
		uint32_t index = toggleFirstInvalid();
		count++;
		data[index] = component;
		return index;
	}
	__forceinline bool getIsValid(uint32_t index) {
		return (bool)isValid[index];
	}
	__forceinline uint32_t getCount() {
		return count;
	}
	// To be used with toggleFirstInvalid.
	__forceinline void incrementCount() {
		count++;
	}
	__forceinline void decrementCount() {
		count--;
	}
	__forceinline T& operator[](uint32_t index) {
		return data[index];
	}
};

template <typename T, uint32_t maxLength>
class FlatBuffer {
	T data[maxLength];
public:
	uint32_t count = 0;
	__forceinline T& operator[](uint32_t index) {
		return data[index];
	}
	__forceinline void push(T _data) {
		data[count] = _data;
		count++;
	}
	/*__forceinline void pushIfUnique(T _data) {
		for (auto i = 0; i < count; i++)
			if (data[i] == _data)
				return;
		push(_data);
	}*/
	__forceinline void pop() {
		count--;
	}
	__forceinline void clear() {
		count = 0;
	}
};