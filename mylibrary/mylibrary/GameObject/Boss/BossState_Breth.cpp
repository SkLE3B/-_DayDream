#include "BossState_Breth.h"
#include "BossState_LockOn.h"
#include "BossState_Wait.h"

void BossState_Breth::Initialize()
{
	step = BrethStep::BrethStart();
	distance = 5.0f;
	velocity = 5.5f;
	forwardVector = { 0,0,1 };
	time.SetMaxTime(15.0f);
}

void BossState_Breth::Update(Player* player, AttackEnemyCollisionObject* ememyCollision, SphereCollider* collider, AudioManager* audio)
{
	time.Update();
	Breth(player, ememyCollision,audio);
	collider->Update();
}

void BossState_Breth::Finalize()
{
}

void BossState_Breth::Breth(Player* player, AttackEnemyCollisionObject* ememyCollision, AudioManager* audio)
{
	if (step == BrethStep::BrethStart)
	{
		audio->PlayWave(L"Resources/sounds/Breth.wav");
		weak_boss.lock()->PlayAnimation(1,1);
		time.Reset();
		step = BrethStep::DuringBreth;
	}
	
	if (time.IsTimer() && step == BrethStep::DuringBreth)
	{					
		if (time.IsTimeOut(10.0f))
		{
			time.Reset();
			ememyCollision->SetPosition(weak_boss.lock()->GetPosition() * distance);
			ememyCollision->ChangeCollisionFlag();
			step = BrethStep::BrethEnd;
		}
	}
	
	if (time.IsTimer() && step == BrethStep::BrethEnd)
	{
		warldMat = weak_boss.lock()->GetMatWorld();	
		distance += velocity;
		walrdPos = warldMat.transformNormal(forwardVector, warldMat);
		walrdPos = { walrdPos.x * distance, 2 ,walrdPos.z * distance };
		PositionCorrection(ememyCollision, walrdPos);
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/Bite.efk", 
			{ ememyCollision->GetPosition().x + walrdPos.x,
			ememyCollision->GetPosition().y + walrdPos.y,ememyCollision->GetPosition().z + walrdPos.z });
		
		if (time.IsTimeOut(1.0f))
		{
			handle = EffekseerManager::StopEffect(handle);
		}

		if (time.IsTimeOut(40.05f))
		{
			//“G‚Ì‹…‚É“–‚½‚Á‚Ä–³‚¢ê‡ƒtƒ‰ƒO‚ğoff‚É‚·‚éB
			if (ememyCollision->GetCollisionFlag())
			{
				ememyCollision->ChangeCollisionFlag();
			}
			weak_boss.lock()->ChangeState(std::make_shared<BossState_Wait>());
		}
	}
}