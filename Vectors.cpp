#include "Vectors.h"

Vector4f::Vector4f() : Vector4f(0, 0, 0, 0){};

Vector4f::Vector4f(Vector3f xyz, float w){
	this->value = { { xyz[0], xyz[1], xyz[2], w } };
}

Vector4f::Vector4f(float x, float y, float z, float w){
	this->value = { { x, y, z, w } };
}

Vector4f Vector4f::operator =(const Vector4f& a){
	if (&a == this){
		return *this;
	}

	this->value = a.value;

	return *this;
}

Vector4f Vector4f::operator =(const float* a){
	this->value = { { a[0], a[1], a[2], a[3] } };

	return *this;
}

Vector4f Vector4f::operator -(const Vector4f& a)const {
	return Vector4f(this->value[0] - a.value[0], this->value[1] - a.value[1], this->value[2] - a.value[2], this->value[3] - a.value[3]);
}
Vector4f Vector4f::operator +(const Vector4f& a)const {
	return Vector4f(this->value[0] + a.value[0], this->value[1] + a.value[1], this->value[2] + a.value[2], this->value[3] + a.value[2]);
}
Vector4f Vector4f::operator *(const float& a) const {
	return Vector4f(this->value[0] * a, this->value[1] * a, this->value[2] * a, this->value[3] * a);
}
Vector4f Vector4f::operator /(const float& a)const {
	return Vector4f(this->value[0] / a, this->value[1] / a, this->value[2] / a, this->value[3] / a);
}

float & Vector4f::operator[](int index){
	return this->value[index];
}
const float & Vector4f::operator[](int index) const{
	return this->value[index];
}



Vector3f::Vector3f() : Vector3f(0, 0, 0){};

Vector3f::Vector3f(float x, float y, float z){
	this->value = { { x, y, z } };
}

Vector3f Vector3f::operator =(const Vector3f& a){
	if (&a == this){
		return *this;
	}

	this->value = a.value;

	return *this;
}

Vector3f Vector3f::operator =(const float* a){
	this->value = { { a[0], a[1], a[2] } };

	return *this;
}

Vector3f Vector3f::operator -(const Vector3f& a)const {
	return Vector3f(this->value[0] - a.value[0], this->value[1] - a.value[1], this->value[2] - a.value[2]);
}
Vector3f Vector3f::operator +(const Vector3f& a)const {
	return Vector3f(this->value[0] + a.value[0], this->value[1] + a.value[1], this->value[2] + a.value[2]);
}
Vector3f Vector3f::operator *(const float& a) const {
	return Vector3f(this->value[0] * a, this->value[1] * a, this->value[2] * a);
}
Vector3f Vector3f::operator /(const float& a)const {
	return Vector3f(this->value[0] / a, this->value[1] / a, this->value[2] / a);
}

/*float operator[](const int index) const{
return this->value[index];
}*/
float & Vector3f::operator[](int index){
	return this->value[index];
}
const float & Vector3f::operator[](int index) const{
	return this->value[index];
}