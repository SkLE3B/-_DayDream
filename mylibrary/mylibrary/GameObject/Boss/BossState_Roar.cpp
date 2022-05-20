#include "BossState_Roar.h"
#include "BossState_LockOn.h"
#include "BossState_Wait.h"

void BossState_Roar::Initialize()
{
	step = RoarStep::RoarStart;
	roarFlag = false;
	bai = 0.0f;
	maxTime = 0.3f;//‘S‘ÌŽžŠÔ[s]
	timeRate = 0;;
	elapsedTime = 0;
	totalTime = 0;
	angle = 0;
	dotPos = 0;
	timerFlag = false;
	roarFlag = false;
	Ppos = {};
	EaseStart = {};
	collisionFlag = false;
	EaseEnd = {};
	forwardVector = { 0,0,1 };
}

void BossState_Roar::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
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
		TimerStart(&time,&timerFlag);
		step = RoarStep::DuringRoar;
	}

	if (timerFlag && step == RoarStep::DuringRoar)
	{
		AdvanceTimer(maxTime);

		if (IsTimeOut(totalTime, 5.5f))
		{
			bai += 15.0f;
			warldMat = weak_boss.lock()->GetMatWorld();
			walrdPos = warldMat.transformNormal(forwardVector, warldMat);
			walrdPos.Normalize();

			walrdPos2 = warldMat.transformNormal(forwardVector, warldMat);
			walrdPos2 = { walrdPos2.x * (bai + 10), 2,walrdPos2.z * (bai + 10) };

			EffekseerManager::SetPosition(handle, weak_boss.lock()->GetPosition() + walrdPos2);
			handle = EffekseerManager::PlayEffect(u"Resources/Effects/Ban.efk", { weak_boss.lock()->GetPosition().x + walrdPos2.x, weak_boss.lock()->GetPosition().y + walrdPos2.y, weak_boss.lock()->GetPosition().z + walrdPos2.z });
			EffekseerManager::SetScale(handle, { 2,2,2 });

			Ppos = player->GetPosition() - weak_boss.lock()->GetPosition();
			Ppos.Normalize();
			dotPos = walrdPos.Dot(Ppos);
			sa = player->GetPosition() - weak_boss.lock()->GetPosition();

			if (sa.z < 0)
			{
				sa.z = sa.z * -1;
			}

			if (dotPos > 0.7f && sa.z < 30)
			{
				roarFlag = !roarFlag;
				player->RoarKnockBack(weak_boss.lock().get(),roarFlag,audio);
			}
			else
			{
				roarFlag = false;
			}
		}

		if (IsTimeOut(totalTime, 5.51f))
		{
			handle = EffekseerManager::StopEffect(handle);
		}
		
		if (IsTimeOut(totalTime, 39.0f))
		{
			ResetTimer();
			step = RoarStep::RoarEnd;
		}
	}

	if (step == RoarStep::RoarEnd)
	{
		weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
	}
}

