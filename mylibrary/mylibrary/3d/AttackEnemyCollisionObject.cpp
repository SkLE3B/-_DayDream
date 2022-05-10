#include "AttackEnemyCollisionObject.h"
#include "../Collision/SphereCollider.h"
#include "../Collision/CollisionAttribute.h"
#include "../Collision/CollisionManager.h"

std::shared_ptr<AttackEnemyCollisionObject> AttackEnemyCollisionObject::Create(Model* model)
{
	std::shared_ptr<AttackEnemyCollisionObject> instance = std::make_shared<AttackEnemyCollisionObject>();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize(model)) {
		instance.reset();
		assert(0);
	}

	return instance;
}

bool AttackEnemyCollisionObject::Initialize(Model* model)
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	collisionFlag = false;

	SetModel(model);
	float radius = 10.5f;
	// �R���C�_�[�̒ǉ�
	SphereCollider* collider = new SphereCollider(DirectX::XMVECTOR{ radius ,radius ,radius ,0 },radius);
	SetCollider(collider, COLLISION_ATTR_ARTSENEMYS);

	return true;
}

void AttackEnemyCollisionObject::Update(Boss* boss,Player* player, AudioManager* audio)
{
	// ���[���h�s��X�V
	UpdateWorldMatrix();
	collider->Update();

	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	//�U������
	class AttckQueryCallback : public QueryCallback
	{
	public:
		AttckQueryCallback(Sphere* sphere) : sphere(sphere) {};

		// �Փˎ��R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info) {

			flag = true;
			return true;
		}

		Sphere* sphere = nullptr;
		bool flag = false;
	};

	AttckQueryCallback callbackAttack(sphereCollider);

	// ���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callbackAttack, COLLISION_ATTR_ALLIES);

	if (boss->GetColFlag() && callbackAttack.flag == true)
	{
		roarFlag = true;
		player->RoarKnockBack(boss, roarFlag,audio);
	}

	// �s��̍X�V�Ȃ�
	Object3d::Update();
}
