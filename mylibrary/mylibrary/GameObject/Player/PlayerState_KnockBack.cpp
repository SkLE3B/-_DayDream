#include "PlayerState_KnockBack.h"
#include "PlayerState_None.h"

void PlayerState_KnockBack::Initialize()
{
	step = KnockBackStep::KnockBackStart;
	timer.SetMaxTime(2.5f);
}

void PlayerState_KnockBack::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	timer.Update();
	if (step == KnockBackStep::KnockBackStart)
	{
		const float knockBackDistance = 35.0f;
		Vector3 distination = weak_player.lock()->GetPosition() - boss->GetPosition();
		distination.Normalize();
		weak_player.lock()->PlayAnimation(5, 0);
		audio->PlayWave(L"Resources/sounds/damage.wav");
		weak_player.lock()->GetHp()->add(-50);
		timer.Reset();
		DecisionDistance(weak_player.lock()->GetBossHit().move + distination * knockBackDistance);
		step = KnockBackStep::DuringKnockBack;
	}

	if (timer.IsTimer() && step == KnockBackStep::DuringKnockBack)
	{
		const float AttackFlame3 = 2.5f;
		Easing(weak_player.lock()->GetPosition(), AttackFlame3);

		if (timer.IsEasingOver())
		{
			weak_player.lock()->ResetAnimation();
			weak_player.lock()->PlayAnimation(0, 0);
			Player::BossHIt::FalseFlag();
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
		}
	}
}

void PlayerState_KnockBack::Finalize()
{
}
