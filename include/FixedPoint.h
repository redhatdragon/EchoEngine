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
	FixedPoint(const FixedPoint& other) {
		uint32_t otherScale = other.getDecScale();
		//TODO: Finish me!
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
	inline std::string getAsString() const {
		std::string retValue = "";
		int32_t whole = data / decScale;
		int32_t remainder = data % decScale;
		//float dec = remainder/decScale;
		int32_t dec = (data*100) / decScale;

		retValue += std::to_string(whole);
		retValue += ".";
		retValue += std::to_string(dec);
		std::cout << whole << std::endl;
		std::cout << remainder << std::endl;
		std::cout << dec << std::endl;
		std::cout << data << std::endl;
		return retValue;

		//data = 1300
		//decScale = 512
		//remainder = 276
		//realResult = 2.5390625
		//result = "253"

		//data = 33
		//decScale = 512
		//remainder = 33
		//realResult = 0.064453125
		//result = "6"
		
		//data = 30
		//decScale = 100
		//remainder = 30
		//result = "3"
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
		if (dotPos == -1) return false;
		int32_t wholeValue = 0;
		for (uint32_t i = 0; i < dotPos; i++) {
			if (str[i] < '0' || str[i] > '9') return false;
			uint32_t digitCount = dotPos - i;
			int32_t digitValue = 1;
			for (uint32_t j = 1; j < digitCount; j++)
				digitValue *= 10;
			wholeValue += (digitValue)*(uint32_t)(str[i] - '0');
		}
		int32_t decimalNumbers = 0;
		/*for (uint32_t i = dotPos + 1; i < str.size(); i++) {
			if (str[i] < '0' || str[i] > '9') return false;
			uint32_t digitCount = (str.size() - 1) - i;
			int32_t digitValue = 1;
			for (uint32_t j = 0; j < digitCount; j++)
				digitValue *= 10;
			decimalNumbers += digitValue * (uint32_t)(str[i] - '0');
		}*/
		for (uint32_t i = dotPos + 1; i < str.size(); i++) {
			if (str[i] < '0' || str[i] > '9') return false;
			uint32_t digitCount = (str.size()) - i;
			int32_t digitValue = 1;
			for (uint32_t j = 1; j < digitCount; j++)
				digitValue *= 10;
			decimalNumbers += digitValue * (uint32_t)(str[i] - '0');
		}

		int32_t totalValue = wholeValue * decScale;
		int32_t decDigitOffset = 1;
		for (uint32_t i = dotPos+1; i < str.size(); i++)
			decDigitOffset *= 10;
		totalValue += ((decimalNumbers * decScale) / decDigitOffset);
		data = totalValue;
		return true;
	}
	inline const uint32_t getDecScale() const {
		return decScale;
	}
};

/*FixedPoint<> sqrt(FixedPoint<> number) {
	//return fp_sqrt(number.getRaw(), 8);
	return sqrt(number.getAsFloat());
}*/
int32_t sqrtI32(int32_t v) {
	uint32_t b = 1 << 30, q = 0, r = v;
	while (b > r)
		b >>= 2;
	while (b > 0) {
		uint32_t t = q + b;
		q >>= 1;
		if (r >= t) {
			r -= t;
			q += b;
		}
		b >>= 2;
	}
	return q;
}
FixedPoint<> sqrt(const FixedPoint<>& var) {
	int32_t value = var.getAsInt();  //or value = input.getAsRaw()/256;
	return sqrtI32(value);  //or retValue.setRaw(sqrt(value)*256);
};



#ifdef TESTING
#include <iostream>
std::string T_FixedPointInitialize() {
	FixedPoint<256> value;
	std::string retValue = "";
	value.fromString("321.123f");
	if (value.getRaw() != 82207) {  //About 321.121 in float
		retValue += "Expected value 321123 but got ";
		retValue += std::to_string(value.getRaw());
		return retValue;
	}
	return "";
}
std::string T_FixedPointGetAsString() {
	FixedPoint<100> value;
	std::string retValue = "";
	value.fromString("0.3f");
	if (value.getAsString() != "0.3f") {
		retValue += "Expected string 0.3f but got ";
		retValue += value.getAsString();
		return retValue;
	}
	return "";
}
/*std::string T_FixedPointSqrt() {
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
}*/
#endif
void testFixedPoint() {
#ifdef TESTING
	std::string dbgStr = "";
	dbgStr = T_FixedPointInitialize();
	if (dbgStr != "") {
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
	dbgStr = T_FixedPointGetAsString();
	if (dbgStr != "") {
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
#endif
}