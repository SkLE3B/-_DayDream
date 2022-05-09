#include "Boss.h"
#include "Collision/CollisionManager.h"
#include "Collision/CollisionAttribute.h"
#include "Collision/SphereCollider.h"
#include "BossState_Wait.h"

using namespace DirectX;

std::shared_ptr<Boss> Boss::Create(Model* model)
{   //3Dオブジェクトのインスタンスを生成
	std::shared_ptr<Boss> instance = std::make_shared<Boss>();
	if (instance == nullptr) {
		return nullptr;
	}

	// 初期化
	if (!instance->Initialize()) {
		instance.reset();
		assert(0);
	}

	if (model) {
		instance->SetModel(model);
	}

	return instance;
}

bool Boss::Initialize()
{
	if (!Object3d::Initialize())
	{
		return false;
	}

	maxTime = 0.3f;//全体時間[s]
	timeRate = 0;;
	elapsedTime = 0;
	totalTime = 0;
	timerFlag = false;
	easing_start = {};
	easing_end = {};
	walrdPosition = {};
	player_position = {};
	angle = 0;
	
	lifeFlag = true;
    ChangeState(std::make_shared<BossState_Wait>());
	forwardVector = { 0,0,1 };
	float bai = 1.0f;
	radius = 10.5f;
	SetCollider(new SphereCollider(XMVECTOR({ radius,radius,radius,0 }), radius), COLLISION_ATTR_ENEMYS);

	return true;
}

void Boss::Update(Player* player,AttackEnemyCollisionObject* AttackCollision, AudioManager* audio)
{
	sphereCollider = dynamic_cast<SphereCollider*>(collider);
	assert(sphereCollider);

	sphereCollider->SetOffset((GetPosition().x, GetPosition().y,GetPosition().z + 30));

	// ワールド行列更新
	UpdateWorldMatrix();
	collider->Update();

	if (shared_bossState && lifeFlag)
	{
		shared_bossState->Update(player, AttackCollision, sphereCollider,audio);
	}

	// 行列の更新など
	Object3d::Update();
}

float& Boss::GetTime()
{
	return shared_bossState.get()->GetTime();
}

bool& Boss::GetRoarFlag()
{
	return shared_bossState.get()->GetRoarFlag(); 
}

bool& Boss::GetColFlag()
{
	return shared_bossState.get()->GetCollisionFlag();
}

void Boss::ChangeState(std::shared_ptr<BossBaseState> State)
{
	if (shared_bossState)
	{
		shared_bossState->Finalize();
	}

	shared_bossState = State;
	shared_bossState->SetBoss(shared_from_this());
	shared_bossState->Initialize();
}
