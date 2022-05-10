#pragma once
#include<DirectXMath.h>
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>

struct Vector3 :public DirectX::XMFLOAT3
{
	Vector3();
	Vector3(const Vector3& vector);
	Vector3(const XMFLOAT3& vector);
	Vector3(const DirectX::XMVECTOR& vector);
	Vector3(const float& num);
	Vector3(const float& x, const float& y, const float& z);

	Vector3& operator+= (Vector3 vector);

	Vector3& operator-= (Vector3 vector);

	Vector3& operator*= (Vector3 vector);

	Vector3& operator/= (Vector3 vector);

	bool operator!= (Vector3 vector);
	bool operator!= (float num);


	Vector3& operator+= (float f);

	Vector3& operator-= (float f);

	Vector3& operator*= (float f);

	Vector3& operator/= (float f);


	Vector3& operator=(float num);

	//í∑Ç≥ÇéÊìæ
	float Length() const;

	//ê≥ãKâª
	Vector3& Normalize();

	//äOêœ
	Vector3 Cross(Vector3 vector);

	//ì‡êœ
	float Dot(Vector3 vector);

	DirectX::XMFLOAT3 ConvertFLOAT3();

	//ê√ìIä÷êî
	static Vector3 Straight(Vector3 myPos, Vector3 otherPos);

	//í∑Ç≥ÇéÊìæ
	static float Length(Vector3 vector);

	//ê≥ãKâª
	static Vector3 Normalize(Vector3 vector);

	//äOêœ
	static Vector3 Cross(Vector3 vector1, Vector3 vector2);

	//ì‡êœ
	static float Dot(Vector3 vector1, Vector3 vector2);

	//XMFLOAT3Ç…ïœä∑
	static DirectX::XMFLOAT3 ConvertFLOAT3(const Vector3& vector);

	//Effekseer::Vector3DÇ…ïœä∑
	static Effekseer::Vector3D ConvertEffeckseerVector3(const Vector3& vector);
};

const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);

const Vector3 operator/(const Vector3& v, float s);
const Vector3 operator/(const Vector3& v, float s);