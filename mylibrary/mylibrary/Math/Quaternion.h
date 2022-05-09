#pragma once
#include "Vector3.h"
#include "Matrix4.h"

struct Quaternion
{
	float x;	//x����
	float y;	//y����
	float z;	//z����
	float w;	//w����

	//�R���X�g���N�^
	Quaternion();
	Quaternion(float x, float y, float z, float w);
};

//�N�H�[�^�j�I�����쐬
Quaternion CreateQuaternion(float x = 0.0f, float y = 0.0f, float z = 0.0f, float w = 1.0f);
Quaternion CreateQuaternion(const Vector3& v, float angle);
Quaternion CreateQuaternion(const Vector3& v, const Quaternion& q);

//���ς����߂�
float Dot(const Quaternion& q1, const Quaternion& q2);

//�m���������߂�
float Length(const Quaternion& q);

//���K������
Quaternion Normalize(const Quaternion& q);

//�����i���傤�₭�j�l���������߂�
Quaternion Conjugate(const Quaternion& q);

//�P�����Z�q�I�[�o�[���[�h
Quaternion operator+(const Quaternion& q);
Quaternion operator-(const Quaternion& q);

//������Z�q�I�[�o�[���[�h
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