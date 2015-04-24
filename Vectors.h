#pragma once
#include <array>

template <typename vecType>
class TVector2{
public:
	TVector2();

	TVector2(vecType x, vecType y);

	TVector2 operator =(const TVector2& a);

	TVector2 operator =(const vecType* a);

	std::array<vecType, 2> value;


	TVector2 operator -(const TVector2& a) const;
	TVector2 operator +(const TVector2& a) const;
	TVector2 operator *(const vecType& a) const;
	TVector2 operator /(const vecType& a) const;

	vecType & operator[](int index);
	const vecType & operator[](int index) const;
};

typedef TVector2<float> Vector2f;
typedef TVector2<int> Vector2d;
typedef TVector2<unsigned char> Vector2b;
typedef TVector2<unsigned int> Vector2ui;

template <typename vecType>
class TVector3{
public:
	TVector3();

	TVector3(vecType x, vecType y, vecType z);

	TVector3 operator =(const TVector3& a);

	TVector3 operator =(const vecType* a);

	std::array<vecType, 3> value;


	TVector3 operator -(const TVector3& a) const;
	TVector3 operator +(const TVector3& a) const;
	TVector3 operator *(const vecType& a) const;
	TVector3 operator /(const vecType& a) const;

	vecType & operator[](int index);
	const vecType & operator[](int index) const;
};

typedef TVector3<float> Vector3f;
typedef TVector3<int> Vector3d;
typedef TVector3<unsigned char> Vector3b;
typedef TVector3<unsigned int> Vector3ui;

template <typename vecType>
class TVector4{
public:
	TVector4();
	TVector4(TVector3<vecType> xyz, vecType w = 1);
	TVector4(vecType x, vecType y, vecType z, vecType w);
	TVector4 operator =(const TVector4& a);
	TVector4 operator =(const vecType* a);


	std::array<vecType, 4> value;


	TVector4 operator -(const TVector4& a) const;
	TVector4 operator +(const TVector4& a) const;
	TVector4 operator *(const vecType& a) const;
	TVector4 operator /(const vecType& a)const;

	vecType & operator[](int index);
	const vecType & operator[](int index) const;
};

typedef TVector4<float> Vector4f;
typedef TVector4<int> Vector4d;
typedef TVector4<unsigned char> Vector4b;
typedef TVector4<unsigned int> Vector4ui;

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

#include "Vectors.tpp"