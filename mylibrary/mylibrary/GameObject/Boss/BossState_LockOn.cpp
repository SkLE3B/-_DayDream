#include "BossState_LockOn.h"
#include "BossState_Wait.h"
#include "BossState_Rush.h"
#include "BossState_BackStep.h"
#include "BossState_Rolling.h"
#include "BossState_Biting.h"
#include "BossState_Breth.h"
#include "BossState_Roar.h"

void BossState_LockOn::Initialize()
{
	step = LockOnStep::LockOnStart;
	maxTime = 0.3f;//�S�̎���[s]
	Ppos = {};
	distanceBoundary = 40;
	time.SetMaxTime(5.0f);
}

void BossState_LockOn::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	time.Update();
	LockOn(player);
	collider->Update();
}

void BossState_LockOn::Finalize()
{
}

void BossState_LockOn::LockOn(Player* player)
{
	if (step == LockOnStep::LockOnStart)
	{
		Ppos = player->GetPosition();
		time.Reset();
		DecisionDistance(Ppos.x, Ppos.z);
		step = LockOnStep::DuringLockOn;
	}

	if (time.IsTimer() && step == LockOnStep::DuringLockOn)
	{
		const float MoveFlame = 2.0f;

		Vector3 bbpos = weak_boss.lock()->GetPosition();
		EasingRot(bbpos, MoveFlame, player,weak_boss.lock()->GetRotation());

		if (time.IsTimeOut(2.0f))
		{
			time.Reset();
			step = LockOnStep::LockOnEnd;
		}
	}

	if (time.IsTimer() && step == LockOnStep::LockOnEnd)
	{
		if (time.IsTimeOut(5.0f))
		{
			SelectState(player);
		}
	}
}

void BossState_LockOn::SelectState(Player* player)
{
	pos  = weak_boss.lock()->GetPosition();
	playerpos = player->GetPosition();

	int minRandom = 0;
	int maxRandom = 0;

	float distance = (pos - playerpos).Length();

	if (distance < distanceBoundary)
	{
		minRandom = 2;
		maxRandom = 5;
	}
	else if (distance > distanceBoundary)
	{
		minRandom = 6;
		maxRandom = 7;
	}

	statePattern = static_cast<BossStatePattern>(MakeRand(maxRandom, minRandom));

	switch (statePattern)
	{
	case BossStatePattern::WAIT:
		break;
	case BossStatePattern::LOCKON:
		weak_boss.lock()->ChangeState(std::make_shared<BossState_LockOn>());
		break;
	case BossStatePattern::RUSH:
		weak_boss.lock()->ChangeState(std::make_shared<BossState_Rush>());
		break;
	case BossStatePattern::BACKSTEP:
		weak_boss.lock()->ChangeState(std::make_shared<BossState_BackStep>());
		break;
	case BossStatePattern::ROLLING:
		weak_boss.lock()->ChangeState(std::make_shared<BossState_Rolling>());
		break;
	case BossStatePattern::BITING:
		weak_boss.lock()->ChangeState(std::make_shared<BossState_Biting>());
		break;
	case BossStatePattern::BRETH:
		weak_boss.lock()->ChangeState(std::make_shared<BossState_Breth>());
		break;
	case BossStatePattern::ROAR:
		weak_boss.lock()->ChangeState(std::make_shared<BossState_Roar>());
		break;
	default:
		break;
	}
}

int BossState_LockOn::MakeRand(int maxNum, int minNum)
{
	int range;
	range = maxNum - minNum + 1;

	int num = 0;
	num = rand() % range + minNum;
	return num;
}