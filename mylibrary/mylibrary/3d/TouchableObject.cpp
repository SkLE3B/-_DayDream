#include "TouchableObject.h"
#include "../Collision/MeshCollider.h"
#include "../Collision/CollisionAttribute.h"

std::shared_ptr<TouchableObject> TouchableObject::Create(Model* model, Camera* camera)
{
	// オブジェクトのインスタンスを生成
	std::shared_ptr<TouchableObject> instance = std::make_shared<TouchableObject>();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize(model,camera)) {
		instance.reset();
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
