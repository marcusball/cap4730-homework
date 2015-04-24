
/*******************************************************/
/** Vector4                                           **/
/*******************************************************/
template <typename vecType>
TVector4<vecType>::TVector4() : TVector4<vecType>(0, 0, 0, 0){};

template <typename vecType>
TVector4<vecType>::TVector4(TVector3<vecType> xyz, vecType w){
	this->value = { { xyz[0], xyz[1], xyz[2], w } };
}

template <typename vecType>
TVector4<vecType>::TVector4(vecType x, vecType y, vecType z, vecType w){
	this->value = { { x, y, z, w } };
}

template <typename vecType>
TVector4<vecType> TVector4<vecType>::operator =(const TVector4<vecType>& a){
	if (&a == this){
		return *this;
	}

	this->value = a.value;

	return *this;
}

template <typename vecType>
TVector4<vecType> TVector4<vecType>::operator =(const vecType* a){
	this->value = { { a[0], a[1], a[2], a[3] } };

	return *this;
}

template <typename vecType>
TVector4<vecType> TVector4<vecType>::operator -(const TVector4<vecType>& a)const {
	return TVector4<vecType>(this->value[0] - a.value[0], this->value[1] - a.value[1], this->value[2] - a.value[2], this->value[3] - a.value[3]);
}

template <typename vecType>
TVector4<vecType> TVector4<vecType>::operator +(const TVector4<vecType>& a)const {
	return TVector4<vecType>(this->value[0] + a.value[0], this->value[1] + a.value[1], this->value[2] + a.value[2], this->value[3] + a.value[2]);
}

template <typename vecType>
TVector4<vecType> TVector4<vecType>::operator *(const vecType& a) const {
	return TVector4<vecType>(this->value[0] * a, this->value[1] * a, this->value[2] * a, this->value[3] * a);
}

template <typename vecType>
TVector4<vecType> TVector4<vecType>::operator /(const vecType& a)const {
	return TVector4<vecType>(this->value[0] / a, this->value[1] / a, this->value[2] / a, this->value[3] / a);
}

template <typename vecType>
vecType & TVector4<vecType>::operator[](int index){
	return this->value[index];
}

template <typename vecType>
const vecType & TVector4<vecType>::operator[](int index) const{
	return this->value[index];
}

template <typename vecType>
float TVector4<vecType>::DistanceTo(const TVector4 & other){
	return std::sqrt(std::pow(other[0] - this->value[0], 2) + std::pow(other[1] - this->value[1], 2) + std::pow(other[2] - this->value[2], 2));
}

/*******************************************************/
/** Vector3                                           **/
/*******************************************************/


template <typename vecType>
TVector3<vecType>::TVector3(){};

template <typename vecType>
TVector3<vecType>::TVector3(vecType x, vecType y, vecType z){
	this->value = { { x, y, z } };
}

template <typename vecType>
TVector3<vecType> TVector3<vecType>::operator =(const TVector3& a){
	if (&a == this){
		return *this;
	}

	this->value = a.value;

	return *this;
}

template <typename vecType>
TVector3<vecType> TVector3<vecType>::operator =(const vecType* a){
	this->value = { { a[0], a[1], a[2] } };

	return *this;
}

template <typename vecType>
TVector3<vecType> TVector3<vecType>::operator -(const TVector3& a)const {
	return TVector3(this->value[0] - a.value[0], this->value[1] - a.value[1], this->value[2] - a.value[2]);
}
template <typename vecType>
TVector3<vecType> TVector3<vecType>::operator +(const TVector3& a)const {
	return TVector3(this->value[0] + a.value[0], this->value[1] + a.value[1], this->value[2] + a.value[2]);
}
template <typename vecType>
TVector3<vecType> TVector3<vecType>::operator *(const vecType& a) const {
	return TVector3(this->value[0] * a, this->value[1] * a, this->value[2] * a);
}
template <typename vecType>
TVector3<vecType> TVector3<vecType>::operator /(const vecType& a)const {
	return TVector3(this->value[0] / a, this->value[1] / a, this->value[2] / a);
}

/*vecType operator[](const int index) const{
return this->value[index];
}*/
template <typename vecType>
vecType & TVector3<vecType>::operator[](int index){
	return this->value[index];
}
template <typename vecType>
const vecType & TVector3<vecType>::operator[](int index) const{
	return this->value[index];
}

/*******************************************************/
/** Vector2                                           **/
/*******************************************************/

template <typename vecType>
TVector2<vecType>::TVector2() : TVector2<vecType>(0, 0){};

template <typename vecType>
TVector2<vecType>::TVector2(vecType x, vecType y){
	this->value = { { x, y } };
}

template <typename vecType>
TVector2<vecType> TVector2<vecType>::operator =(const TVector2<vecType>& a){
	if (&a == this){
		return *this;
	}

	this->value = a.value;

	return *this;
}

template <typename vecType>
TVector2<vecType> TVector2<vecType>::operator =(const vecType* a){
	this->value = { { a[0], a[1] } };

	return *this;
}

template <typename vecType>
TVector2<vecType> TVector2<vecType>::operator -(const TVector2<vecType>& a)const {
	return TVector2<vecType>(this->value[0] - a.value[0], this->value[1] - a.value[1]);
}
template <typename vecType>
TVector2<vecType> TVector2<vecType>::operator +(const TVector2<vecType>& a)const {
	return TVector2<vecType>(this->value[0] + a.value[0], this->value[1] + a.value[1]);
}
template <typename vecType>
TVector2<vecType> TVector2<vecType>::operator *(const vecType& a) const {
	return TVector2<vecType>(this->value[0] * a, this->value[1] * a);
}
template <typename vecType>
TVector2<vecType> TVector2<vecType>::operator /(const vecType& a)const {
	return TVector2<vecType>(this->value[0] / a, this->value[1] / a);
}

/*vecType operator[](const int index) const{
return this->value[index];
}*/
template <typename vecType>
vecType & TVector2<vecType>::operator[](int index){
	return this->value[index];
}
template <typename vecType>
const vecType & TVector2<vecType>::operator[](int index) const{
	return this->value[index];
}
