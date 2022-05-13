#include "AttackCollisionObject.h"
#include "SphereCollider.h"
#include "../Collision/CollisionAttribute.h"
#include "../Collision/CollisionManager.h"

std::shared_ptr<AttackCollisionObject> AttackCollisionObject::Create(Model* model, Camera* camera)
{
	// �I�u�W�F�N�g�̃C���X�^���X�𐶐�
	std::shared_ptr<AttackCollisionObject> instance = std::make_shared<AttackCollisionObject>();
	if (instance == nullptr) {
		return nullptr;
	}

	// ������
	if (!instance->Initialize(model, camera)) {
	    instance.reset();
		assert(0);
	}

	return instance;
}

bool AttackCollisionObject::Initialize(Model* model, Camera* camera)
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	collisionFlag = false;
	soundFlag = false;
	soundTimer = 0;

	SetModel(model);
	float radius = 20.5f;
	// �R���C�_�[�̒ǉ�
	SphereCollider* collider = new SphereCollider(DirectX::XMVECTOR{ radius ,radius ,radius ,0});
	SetCollider(collider, COLLISION_ATTR_ARTS);
	HP = new ExNum(100,0,100);

	return true;
}

void AttackCollisionObject::Update(Player* player, ParticleManager* particleMan, AudioManager* audio)
{
	// ���[���h�s��X�V
	UpdateWorldMatrix();

	SphereCollider* sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	// �s��̍X�V�Ȃ�
	Object3d::Update();

	collider->Update();

	//�U������
	class AttckQueryCallback : public QueryCallback
	{
	public:
		AttckQueryCallback(Sphere* sphere) : sphere(sphere) {};

		// �Փˎ��R�[���o�b�N�֐�
		bool OnQueryHit(const QueryHit& info) {
			damage += 1;
			Hit = true;

			return true;
		}

		Sphere* sphere = nullptr;
		int damage = 0;
		bool Hit = false;
	};

	AttckQueryCallback callbackAttack(sphereCollider);

	// ���ƒn�`�̌�����S����
	CollisionManager::GetInstance()->QuerySphere(*sphereCollider, &callbackAttack, COLLISION_ATTR_ENEMYS);
	
	if (player->GetColFlag() && callbackAttack.Hit && !soundFlag)
	{
		callbackAttack.Hit = false;
		HP->add(-0.5f);
		soundFlag = !soundFlag;
		CreateParticles(particleMan);
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/hit.efk", { GetPosition().x, GetPosition().y,GetPosition().z + 5});
		EffekseerManager::SetScale(handle,{3,3,3});
	}

	if (soundFlag)
	{
		soundTimer++;

		if (soundTimer >= 8)
		{
			audio->PlayWave(L"Resources/sounds/Hit.wav");
			handle = EffekseerManager::StopEffect(handle);
			soundFlag = !soundFlag;
			soundTimer = 0;
		}
	}

	// ���[���h�s��X�V
	UpdateWorldMatrix();
	collider->Update();
}

void AttackCollisionObject::CreateParticles(ParticleManager* particleMan)
{
	for (int i = 0; i < 20; i++) {
		// X,Y,Z�S��[-5.0f,+5.0f]�Ń����_���ɕ��z
		const float rnd_pos = 15.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;

		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.051f;
		acc.y -= -(float)rand() / RAND_MAX * rnd_acc;
		acc.z += -(float)rand() / RAND_MAX * rnd_acc;

		XMFLOAT3 po = GetPosition();

		po.y += 3;

		// �ǉ�
		particleMan->Add(60, po, vel, acc, 1.0f, 0.5f);
	}
}

void AttackCollisionObject::CreateParticlesAttack(ParticleManager* particleMan)
{
	for (int i = 0; i < 10; i++) {
		// X,Y,Z�S��[-5.0f,+5.0f]�Ń����_���ɕ��z
		const float rnd_pos = 15.0f;
		XMFLOAT3 pos{};
		pos.y = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;

		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.051f;
		acc.y -= -(float)rand() / RAND_MAX * rnd_acc;
		acc.z += -(float)rand() / RAND_MAX * rnd_acc;

		XMFLOAT3 po = GetPosition();

		// �ǉ�
		particleMan->Add(60, po, vel, acc, 1.0f, 0.5f);
	}
}

AttackCollisionObject * AttackCollisionObject::GetInstance()
{
	static AttackCollisionObject instance;

	return &instance;
}
