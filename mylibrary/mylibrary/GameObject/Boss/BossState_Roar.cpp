#include "BossState_Roar.h"
#include "BossState_LockOn.h"
#include "BossState_Wait.h"
#include "../mylibrary/GameObject/Player/PlayerState_RoarKnockBack.h"

void BossState_Roar::Initialize()
{
	step = RoarStep::RoarStart;
	distance = 0.0f;
	maxTime = 0.3f;//‘S‘ÌŽžŠÔ[s]
	dotPos = 0;
	Ppos = {};
	forwardVector = { 0,0,1 };
	time.SetMaxTime(15.0f);
}

void BossState_Roar::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	time.Update();
	Roar(player,audio);
	collider->Update();
}

void BossState_Roar::Finalize()
{
}

void BossState_Roar::Roar(Player* player, AudioManager* audio)
{
	if (step == RoarStep::RoarStart)
	{
		audio->PlayWave(L"Resources/sounds/Roar.wav");
		weak_boss.lock()->PlayAnimation(2,1);
		time.Reset();
		step = RoarStep::DuringRoarStart;
	}

	if (time.IsTimer() && step == RoarStep::DuringRoarStart)
	{
		if (time.IsTimeOut(10.0f))
		{
			warldMat = weak_boss.lock()->GetMatWorld();
			walrdPos = warldMat.transformNormal(forwardVector, warldMat);
			walrdPos.Normalize();

			const Vector3 effectPosition = { 10,0,40 };
			walrdPos2 = warldMat.transformNormal(forwardVector, warldMat);
			walrdPos2 = { walrdPos2.x * effectPosition.x, 2,walrdPos2.z * effectPosition.z};
			EffekseerManager::SetPosition(handle, weak_boss.lock()->GetPosition() + walrdPos2);
			handle = EffekseerManager::PlayEffect(u"Resources/Effects/roar.efk", { weak_boss.lock()->GetPosition().x + walrdPos2.x, weak_boss.lock()->GetPosition().y + walrdPos2.y, weak_boss.lock()->GetPosition().z + walrdPos2.z });
			EffekseerManager::SetScale(handle, { 6,6,6 });
			Ppos = player->GetPosition() - weak_boss.lock()->GetPosition();
			Ppos.Normalize();
			dotPos = walrdPos.Dot(Ppos);
			CalculatedPos = player->GetPosition() - weak_boss.lock()->GetPosition();
			step = RoarStep::DuringRoar;
			time.Reset();
		}
	}

	if (time.IsTimer() && step == RoarStep::DuringRoar)
	{
		if (CalculatedPos.z < 0)
		{
			CalculatedPos.z = CalculatedPos.z * -1;
		}

		if (weak_boss.lock()->GetRoarFlag() == false && dotPos > 0.7f && CalculatedPos.z < 80)
		{
			weak_boss.lock()->ChangeRoarFlag();
			player->GetHp()->add(-50);
			player->ChangeState(std::make_shared<PlayerState_RoarKnockBack>());
			step = RoarStep::RoarEnd;
		}

		if (time.IsTimeOut(28.0f))
		{
			handle = EffekseerManager::StopEffect(handle);
			weak_boss.lock()->ChangeRoarFlag();
			step = RoarStep::RoarEnd;
		}
	}

	if (step == RoarStep::RoarEnd)
	{
		weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
	}
}