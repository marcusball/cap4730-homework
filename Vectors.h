#pragma once
#include <array>

class Vector3f{
public:
	Vector3f();

	Vector3f(float x, float y, float z);

	Vector3f operator =(const Vector3f& a);

	Vector3f operator =(const float* a);

	std::array<float, 3> value;


	Vector3f operator -(const Vector3f& a) const;
	Vector3f operator +(const Vector3f& a) const;
	Vector3f operator *(const float& a) const;
	Vector3f operator /(const float& a) const;

	float & operator[](int index);
	const float & operator[](int index) const;
};

class Vector4f{
public:
	Vector4f();
	Vector4f(Vector3f xyz, float w = 1.f);
	Vector4f(float x, float y, float z, float w);
	Vector4f operator =(const Vector4f& a);
	Vector4f operator =(const float* a);


	std::array<float, 4> value;


	Vector4f operator -(const Vector4f& a) const;
	Vector4f operator +(const Vector4f& a) const;
	Vector4f operator *(const float& a) const;
	Vector4f operator /(const float& a)const;

	float & operator[](int index);
	const float & operator[](int index) const;
};