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

	//長さを取得
	float Length() const;

	//正規化
	Vector3& Normalize();

	//外積
	Vector3 Cross(Vector3 vector);

	//内積
	float Dot(Vector3 vector);

	DirectX::XMFLOAT3 ConvertFLOAT3();

	//静的関数
	static Vector3 Straight(Vector3 myPos, Vector3 otherPos);

	//長さを取得
	static float Length(Vector3 vector);

	//正規化
	static Vector3 Normalize(Vector3 vector);

	//外積
	static Vector3 Cross(Vector3 vector1, Vector3 vector2);

	//内積
	static float Dot(Vector3 vector1, Vector3 vector2);

	//XMFLOAT3に変換
	static DirectX::XMFLOAT3 ConvertFLOAT3(const Vector3& vector);

	//Effekseer::Vector3Dに変換
	static Effekseer::Vector3D ConvertEffeckseerVector3(const Vector3& vector);
};

const Vector3 operator+(const Vector3& v1, const Vector3& v2);
const Vector3 operator-(const Vector3& v1, const Vector3& v2);
const Vector3 operator*(const Vector3& v, float s);
const Vector3 operator*(float s, const Vector3& v);
const Vector3 operator*(const Vector3& v1, const Vector3& v2);

const Vector3 operator/(const Vector3& v, float s);
const Vector3 operator/(const Vector3& v, float s);