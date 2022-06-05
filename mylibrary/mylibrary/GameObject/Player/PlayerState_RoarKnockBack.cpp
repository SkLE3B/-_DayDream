#include "PlayerState_RoarKnockBack.h"
#include "PlayerState_None.h"

void PlayerState_RoarKnockBack::Initialize()
{
	step = RoarKnockBackStep::RoarKnockBackStart;
}

void PlayerState_RoarKnockBack::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	if (boss->GetRoarFlag() && step == RoarKnockBackStep::RoarKnockBackStart)
	{
		float knockBackDistance = 35.0f;
		Vector3 distination = weak_player.lock()->GetPosition() - boss->GetPosition();
		distination.Normalize();
		float roarMove = 1.1f;
		audio->PlayWave(L"Resources/sounds/damage.wav");
		weak_player.lock()->PlayAnimation(5, 0);
		HelperTimer::TimerStart();
		DecisionDistance(distination * knockBackDistance);
		step = RoarKnockBackStep::RoarDuringKnockBack;
	}

	if (step == RoarKnockBackStep::RoarDuringKnockBack)
	{
		const float AttackFlame3 = 1.5f;
		Easing(weak_player.lock()->GetPosition(), AttackFlame3);
		HelperTimer::AdvanceTimer(AttackFlame3);

		if (HelperTimer::IsEasingOver())
		{
			weak_player.lock()->ResetAnimation();
			HelperTimer::ResetTimer();
			weak_player.lock()->PlayAnimation(0, 0);
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
		}
	}
}

void PlayerState_RoarKnockBack::Finalize()
{
}