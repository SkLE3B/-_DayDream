#pragma once
#include "Vector3.h"
#include "Matrix4.h"

struct Quaternion
{
	float x;	//x成分
	float y;	//y成分
	float z;	//z成分
	float w;	//w成分

	//コンストラクタ
	Quaternion();
	Quaternion(float x, float y, float z, float w);
};

//クォータニオンを作成
Quaternion CreateQuaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
Quaternion CreateQuaternion(const Vector3& v, float angle);
Quaternion CreateQuaternion(const Vector3& v, const Quaternion& q);

//内積を求める
float Dot(const Quaternion& q1, const Quaternion& q2);

//ノルムを求める
float Length(const Quaternion& q);

//正規化する
Quaternion Normalize(const Quaternion& q);

//共役（きょうやく）四元数を求める
Quaternion Conjugate(const Quaternion& q);

//単項演算子オーバーロード
Quaternion operator+(const Quaternion& q);
Quaternion operator-(const Quaternion& q);

//代入演算子オーバーロード
Quaternion& operator+=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator-=(Quaternion& q1, const Quaternion& q2);
Quaternion& operator*=(Quaternion& q, float s);
Quaternion& operator/=(Quaternion& q, float s);
Quaternion& operator*=(Quaternion& q1, const Quaternion& q2);

Quaternion operator+(const Quaternion& q1, const Quaternion& q2);
Quaternion operator-(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q1, const Quaternion& q2);
Quaternion operator*(const Quaternion& q, float s);
Quaternion operator*(float s, const Quaternion& q);
Quaternion operator/(const Quaternion& q, float s);

Quaternion Sleap(const Quaternion& q1, const Quaternion& q2, float t);
Quaternion Leap(const Quaternion& q1, const Quaternion& q2, float t);
//Matrix4 Rotate(const Quaternion& q);
Quaternion CreatequaternionFromRotMat(const Matrix4& m);
Vector3 GetAxis(const Quaternion& q);