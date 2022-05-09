#pragma once
#include <DirectXMath.h>
#include "Vector3.h"

// MATRIX �^�� Matrix4 �^�Ƃ��Ĉ��� ���݊����̊m��&�g���̂���
//using Matrix4 = DirectX::XMMATRIX;

struct Matrix4 : public DirectX::XMMATRIX
{
	////�P�ʍs������߂�
	//Matrix4 identity();

	////�g��k���s��̐ݒ�
	//Matrix4 scale(const Vector3& s);

	////��]�s��̐ݒ�
	//Matrix4 rotateX(float angle);
	//Matrix4 rotateY(float angle);
	//Matrix4 rotateZ(float angle);

	////���s�ړ��s��̍쐬
	//Matrix4 translate(const Vector3& t);

	////���W�ϊ��i�x�N�g���ƍs��̊|���Z������j
	//Vector3 transform(const Vector3& v, const Matrix4& m);

	//�x�N�g���ϊ��i�x�N�g���ƍs��̊|���Z������j
	Vector3 transformNormal(const Vector3& v, const Matrix4& m);
};