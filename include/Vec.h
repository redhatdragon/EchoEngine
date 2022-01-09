#pragma once
#include "FixedPoint.h"

template<typename T>
struct Vec2D {
	T x, y;
	__forceinline void operator+=(Vec2D& other) {
		x += other.x;
		y += other.y;
	}
	__forceinline void operator-=(Vec2D& other) {
		x -= other.x;
		y -= other.y;
	}
	__forceinline void operator/=(T num) {
		x /= num;
		y /= num;
	}
	__forceinline void operator*=(T num) {
		x = (x * num);
		y = (y * num);
	}
	__forceinline Vec2D<T> operator+(Vec2D& other) {
		return { x + other.x, y + other.y };
	}
	__forceinline Vec2D<T> operator-(Vec2D& other) {
		return { x - other.x, y - other.y };
	}
	__forceinline Vec2D<T> operator/(T num) {
		return { x / num, y / num };
	}
	__forceinline Vec2D<T> operator*(T num) {
		return { x * num, y * num };
	}
	__forceinline bool isZero() {
		if (x == 0 && y == 0)
			return true;
		return false;
	}
	__forceinline void normalize(uint32_t unit = 1) {
		T dist = getDistance();
		if (dist == 0) {
			x = y = 0;
			return;
		}
		x /= dist / unit; y /= dist / unit;
	}
	__forceinline T getDistance() {
		return sqrt((x * x) + (y * y));
	}
	__forceinline uint64_t getDistanceSquared() {
		return (x * x) + (y * y);
	}
	__forceinline T getDistanceFrom(Vec2D& other) {
		return (*this - other).getDistance();
	}
	__forceinline uint64_t getDistanceFromSquared(Vec2D& other) {
		return (*this - other).getDistanceSquared();
	}
	template<typename T>
	std::string getDebugStr() {
		std::string retValue = std::to_string(x);
		retValue += ", ";
		retValue += std::to_string(y);
		return retValue;
	}
	template<>
	std::string getDebugStr<FixedPoint<>>()	{
		std::string retValue = x.getAsString();
		retValue += ", ";
		retValue += y.getAsString();
		return retValue;
	}
	/*template<>
	std::string getDebugStr<float>() {
		std::string retValue = std::to_string(x);
		retValue += ", ";
		retValue += std::to_string(y);
		return retValue;
	}*/
};



#ifdef TESTING
std::string T_Vec2DNormalizeFixedPoint() {
	Vec2D<FixedPoint<>> vec = { 5, 10 };
	vec.normalize();
	if (vec.getDebugStr<FixedPoint<>>() != "0.5, 1") {
		std::string retValue = "Expected value ";
		retValue += "0.5, 1";
		retValue += " instead got ";
		retValue += vec.getDebugStr<FixedPoint<>>();
		return retValue;
	}
	return "";
}
std::string T_Vec2DNormalize() {
	Vec2D<float> vec = { 5, 10 };
	vec.normalize();
	if (vec.getDebugStr<float>() != "0.5, 1") {
		std::string retValue = "Expected value ";
		retValue += "0.5, 1";
		retValue += " instead got ";
		retValue += vec.getDebugStr<float>();
		return retValue;
	}
	return "";
}
std::string T_Vec2DNormalizeDouble() {
	Vec2D<double> vec = { 5, 10 };
	vec.normalize();
	if (vec.getDebugStr<double>() != "0.5, 1") {
		std::string retValue = "Expected value ";
		retValue += "0.5, 1";
		retValue += " instead got ";
		retValue += vec.getDebugStr<double>();
		return retValue;
	}
	return "";
}
#endif
void testVec2D() {
#ifdef TESTING
	std::string dbgStr = "";
	dbgStr = T_Vec2DNormalizeFixedPoint();
	if (dbgStr != "") {
		std::cout << "T_Vec2DNormalizeFixedPoint(): ";
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
	dbgStr = T_Vec2DNormalize();
	if (dbgStr != "") {
		std::cout << "T_Vec2DNormalize(): ";
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
	dbgStr = T_Vec2DNormalizeDouble();
	if (dbgStr != "") {
		std::cout << "T_Vec2DNormalizeDouble(): ";
		std::cout << dbgStr << std::endl;
		dbgStr = "";
	}
#endif
}