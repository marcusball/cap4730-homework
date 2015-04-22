#pragma once
#include <array>

class Vector2f{
public:
	Vector2f();

	Vector2f(float x, float y);

	Vector2f operator =(const Vector2f& a);

	Vector2f operator =(const float* a);

	std::array<float, 2> value;


	Vector2f operator -(const Vector2f& a) const;
	Vector2f operator +(const Vector2f& a) const;
	Vector2f operator *(const float& a) const;
	Vector2f operator /(const float& a) const;

	float & operator[](int index);
	const float & operator[](int index) const;
};

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

namespace ColorVectors{
	static const Vector4f RED = Vector4f(1.f, 0.f, 0.f, 1.f);
	static const Vector4f ORANGE = Vector4f(1.f, 0.6f, 0.f, 1.f);
	static const Vector4f YELLOW = Vector4f(1.f, 1.f, 0.f, 1.f);
	static const Vector4f GREEN = Vector4f(0.f, 1.f, 0.f, 1.f);
	static const Vector4f CYAN = Vector4f(0.f, 1.f, 1.f, 1.f);
	static const Vector4f BLUE = Vector4f(0.f, 0.f, 1.f, 1.f);
	static const Vector4f PURPLE = Vector4f(1.f, 0.f, 1.f, 1.f);
	static const Vector4f WHITE = Vector4f(1.f, 1.f, 1.f, 1.f);
	static const Vector4f BLACK = Vector4f(0.f, 0.f, 0.f, 1.f);
	static const Vector4f GREY50 = Vector4f(.5f, .5f, .5f, 1.f);
};