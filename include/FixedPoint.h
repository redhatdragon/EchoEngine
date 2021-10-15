#pragma once
#include <stdint.h>
#include <string>
#include "RockBox/fixedpoint.h"

#define FIXED_POINT_DEFAULT_SCALE 256

template <uint32_t decScale = FIXED_POINT_DEFAULT_SCALE>
class FixedPoint {
	int32_t data;
public:
	FixedPoint() {
		data = 0;
	}
	FixedPoint(uint32_t other) {
		data = other * decScale;
	}
	FixedPoint(int32_t other) {
		data = other * decScale;
	}
	FixedPoint(float other) {
		data = other * decScale;
	}
	inline void operator+=(FixedPoint<decScale> other) {
		data += other.data;
	}
	inline void operator-=(FixedPoint<decScale> other) {
		data -= other.data;
	}
	inline void operator*=(FixedPoint<decScale> other) {
		data *= other.data;
	}
	inline void operator/=(FixedPoint<decScale> other) {
		data /= other.data;
	}
	FixedPoint<> operator+(const FixedPoint<decScale> other) const {
		return data + other.data;
	}
	FixedPoint<> operator-(const FixedPoint<decScale> other) const {
		return data - other.data;
	}
	FixedPoint<> operator*(const FixedPoint<decScale> other) const {
		return data * other.data;
	}
	FixedPoint<> operator/(const FixedPoint<decScale> other) const {
		return data / other.data;
	}
	inline void operator++() {
		data += decScale;
	}
	//TODO: are these needed?
	inline FixedPoint<decScale> operator+(FixedPoint<decScale> other) {
		FixedPoint<decScale> retValue;
		retValue.setRaw(this->data);
		retValue += other;
		return retValue;
	}
	inline FixedPoint<decScale> operator-(FixedPoint<decScale> other) {
		FixedPoint<decScale> retValue;
		retValue.setRaw(this->data);
		retValue -= other;
		return retValue;
	}
	inline FixedPoint<decScale> operator*(FixedPoint<decScale> other) {
		FixedPoint<decScale> retValue;
		retValue.setRaw(this->data);
		retValue *= other;
		return retValue;
	}
	inline FixedPoint<decScale> operator/(FixedPoint<decScale> other) {
		FixedPoint<decScale> retValue;
		retValue.setRaw(this->data);
		retValue /= other;
		return retValue;
	}
	inline bool operator<=(FixedPoint<decScale> other) const {
		if (this->data <= other.data) return true;
		return false;
	}
	inline bool operator>=(FixedPoint<decScale> other) const {
		if (this->data >= other.data) return true;
		return false;
	}
	inline bool operator==(FixedPoint<decScale> other) const {
		if (this->data == other.data) return true;
		return false;
	}
	inline bool operator<(FixedPoint<decScale> other) const {
		if (this->data < other.data) return true;
		return false;
	}
	inline bool operator>(FixedPoint<decScale> other) const {
		if (this->data > other.data) return true;
		return false;
	}
	inline bool operator!=(FixedPoint<decScale> other) const {
		if (this->data != other.data) return true;
		return false;
	}
	inline int32_t getAsInt() const {
		return data / decScale;
	}
	inline float getAsFloat() const {
		return ((float)data) / decScale;
	}
	inline int32_t getRaw() const {
		return data;
	}
	inline void setRaw(int32_t other) {
		data = other;
	}
	inline bool fromString(const std::string& _str) {
		std::string str = _str;
		char lastChar = str[str.size() - 1];
		if (lastChar != 'f' && lastChar != 'F')
			return false;
		str.resize(str.size() - 1);
		int32_t dotPos = -1;
		uint32_t strSize = (uint32_t)str.size();
		for (uint32_t i = 0; i < strSize; i++) {
			if (str[i] == '.') {
				dotPos = i;
				break;
			}
		}
		int32_t wholeValue = 0;
		for (uint32_t i = 0; i < dotPos; i++) {
			if (str[i] < '0' || str[i] > '9') return false;
			uint32_t digitCount = dotPos - i;
			int32_t digitValue = 1;
			for (uint32_t j = 0; j < digitCount; j++)
				digitValue *= 10;
			wholeValue += digitValue*(uint32_t)(str[i] - '0');
		}
		int32_t decimalNumbers = 0;
		for (uint32_t i = dotPos + 1; i < str.size(); i++) {
			if (str[i] < '0' || str[i] > '9') return false;
			uint32_t digitCount = (str.size() - 1) - i;
			int32_t digitValue = 1;
			for (uint32_t j = 0; j < digitCount; j++)
				digitValue *= 10;
			decimalNumbers += digitValue * (uint32_t)(str[i] - '0');
		}

		int32_t totalValue = wholeValue * decScale;
		int32_t decDigitOffset = 1;
		for (uint32_t i = dotPos+1; i < str.size() - 1; i++)
			decDigitOffset *= 10;
		totalValue += ((decimalNumbers * decScale) / decDigitOffset);
		data = totalValue;
		return true;
	}
};

FixedPoint<> sqrt(FixedPoint<> number) {
	//return fp_sqrt(number.getRaw(), 8);
	return sqrt(number.getAsFloat());
}



#ifdef TESTING
#include <iostream>
std::string T_FixedPointInitialize() {
	FixedPoint<100> value;
	std::string retValue = "";
	value.fromString("321.123f");
	if (value.getRaw() != 321123) {
		retValue += "Expected value 321123 but got ";
		retValue += std::to_string(value.getRaw());
		return retValue;
	}
	return "";
}
std::string T_FixedPointBigInitialize() {
	
	
	return "";
}
std::string T_FixedPointSqrt() {
	FixedPoint<> value;
	std::string retValue = "";
	value.fromString("9.5f"); 
	value.setRaw(value.getRaw() ^ 2);
	if (sqrt(value).getAsFloat() != sqrt(value.getAsFloat())) {
		retValue += "Expected value ";
		retValue += std::to_string(sqrt(value.getAsFloat()));
		retValue += " instead got ";
		retValue += std::to_string(sqrt(value).getAsFloat());
	}
	return retValue;
}
#endif
void testFixedPoint() {
#ifdef TESTING
	std::string dbgStr = "";
	dbgStr = T_FixedPointInitialize();
	if (dbgStr != "") {
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
	dbgStr = T_FixedPointSqrt();
	if (dbgStr != "") {
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
#endif
}