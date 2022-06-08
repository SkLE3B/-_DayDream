#include "PlayerState_RoarKnockBack.h"
#include "PlayerState_None.h"

void PlayerState_RoarKnockBack::Initialize()
{
	step = RoarKnockBackStep::RoarKnockBackStart;
	timer.SetMaxTime(1.5f);
}

void PlayerState_RoarKnockBack::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	timer.Update();
	if (boss->GetRoarFlag() && step == RoarKnockBackStep::RoarKnockBackStart)
	{
		float knockBackDistance = 35.0f;
		Vector3 distination = weak_player.lock()->GetPosition() - boss->GetPosition();
		distination.Normalize();
		float roarMove = 1.1f;
		audio->PlayWave(L"Resources/sounds/damage.wav");
		weak_player.lock()->PlayAnimation(5, 0);
		timer.Reset();
		DecisionDistance(distination * knockBackDistance);
		step = RoarKnockBackStep::RoarDuringKnockBack;
	}

	if (timer.IsTimer() && step == RoarKnockBackStep::RoarDuringKnockBack)
	{
		const float AttackFlame3 = 1.5f;
		Easing(weak_player.lock()->GetPosition(), AttackFlame3);

		if (timer.IsEasingOver())
		{
			weak_player.lock()->ResetAnimation();
			weak_player.lock()->PlayAnimation(0, 0);
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
		}
	}
}

void PlayerState_RoarKnockBack::Finalize()
{
}