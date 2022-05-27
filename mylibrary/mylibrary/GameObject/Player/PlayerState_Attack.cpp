#include "PlayerState_Attack.h"
#include "PlayerState_None.h"
#include "PlayerState_KnockBack.h"
#include "../mylibrary/Base/HelperTimer.h"

void PlayerState_Attack::Initialize()
{
	step = AttackStep::AttackStart;
	moveDirection = 0;
}

void PlayerState_Attack::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	if (step == AttackStep::AttackStart)
	{
		weak_player.lock()->PlayAnimation(4, 0);
		audio->PlayWave(L"Resources/sounds/swing1.wav");
		HelperTimer::TimerStart();
		Vector3 correctionPos = { 0,2,0 };
		PositionCorrection(AttackCol, GetCorrectionPos(6.0f, correctionPos));
		DecisionDistance(weak_player.lock()->GetMoveDirection() * 30);
		AttackCol->ChangeColFlag();
		step = AttackStep::DuringAttack;
	}	

	if (&HelperTimer::IsTimerOn && step == AttackStep::DuringAttack)
	{
		const float AttackFlame3 = 1.6f;
		HelperTimer::AdvanceTimer(AttackFlame3);
		Easing(AttackCol->GetPosition(), AttackFlame3);
		AttackCol->SetPosition(AttackCol->GetPosition());
		
		if (HelperTimer::IsEasingOver())
		{
			weak_player.lock()->ResetAnimation();
			HelperTimer::ResetTimer();
			weak_player.lock()->PlayAnimation(0, 0);
			AttackCol->ChangeColFlag();
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
		}
	}

	//ノックバック
	if (Player::BossHIt::IsFlag())
	{
		AttackCol->ChangeColFlag();
		Player::BossHIt::FalseFlag();
		HelperTimer::ResetTimer();
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_KnockBack>());
	}
}

void PlayerState_Attack::Finalize()
{
}
