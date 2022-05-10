#include "TouchableObject.h"
#include "../Collision/MeshCollider.h"
#include "../Collision/CollisionAttribute.h"

TouchableObject* TouchableObject::Create(Model* model, Camera* camera)
{
	// オブジェクトのインスタンスを生成
	TouchableObject* instance = new TouchableObject();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(model, camera)) {
		delete instance;
		assert(0);
	}

	return instance;
}

bool TouchableObject::Initialize(Model* model,Camera* camera)
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	SetModel(model);

	// コライダーの追加
	MeshCollider* collider = new MeshCollider;
	SetCollider(collider, COLLISION_ATTR_LANDSHAPE);
	collider->ConstructTriangles(model);
	//collider->SetAttribute(COLLISION_ATTR_LANDSHAPE);

	return true;
}
