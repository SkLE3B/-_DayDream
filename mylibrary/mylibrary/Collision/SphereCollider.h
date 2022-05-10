#pragma once
#include "BaseCollider.h"
#include "CollisionPrimitive.h"
#include "../Math/Vector3.h"

#include <DirectXMath.h>

/// <summary>
/// 球衝突判定オブジェクト
/// </summary>
class SphereCollider : public BaseCollider, public Sphere
{
private: // エイリアス
	// DirectX::を省略
	using XMVECTOR = DirectX::XMVECTOR;
public:
	SphereCollider(Vector3 offset = {0,0,0}, float radius = 2.0f) :
		testOffset(offset),
		radius(radius)
	{
		// 球形状をセット
		shapeType = COLLISIONSHAPE_SPHERE;
	}

	/// <summary>
	/// 更新
	/// </summary>
	void Update() override;

	inline const XMVECTOR& GetOffset() { return offset; }

	inline void SetOffset(const XMVECTOR& offset) { this->offset = offset; }

	inline void SetOffset(const Vector3& offset) { this->testOffset = testOffset; }

	inline float GetRadius() { return radius; }

	inline void SetRadius(float radius) { this->radius = radius; }

private:
	// オブジェクト中心からのオフセット
	XMVECTOR offset;

	Vector3 testOffset;
	// 半径
	float radius;
};