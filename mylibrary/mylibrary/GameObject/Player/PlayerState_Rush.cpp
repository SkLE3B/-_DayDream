#include "PlayerState_Rush.h"
#include "PlayerState_None.h"
#include "PlayerState_KnockBack.h"

void PlayerState_Rush::Initialize()
{
	step = RushStep::RushStart;
}

void PlayerState_Rush::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	if (step == RushStep::RushStart)
	{
		weak_player.lock()->PlayAnimation(1, 0);
		audio->PlayWave(L"Resources/sounds/Drill.wav");
		HelperTimer::TimerStart();
		DecisionDistance(weak_player.lock()->GetMoveDirection() * 60);
		AttackCol->ChangeColFlag();
		step = RushStep::DuringRush;
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/movePlayer.efk", { weak_player.lock()->GetPosition().x,
			weak_player.lock()->GetPosition().y + 5, weak_player.lock()->GetPosition().z * weak_player.lock()->GetMoveDirection().z});
	}

	if (&HelperTimer::IsTimerOn && step == RushStep::DuringRush)
	{
		const float AttackFlame2 = 2.09f;
		Vector3 correctionPos = { 0,2,0 };
		HelperTimer::AdvanceTimer(AttackFlame2);
		Easing(weak_player.lock()->GetPosition(), AttackFlame2);
		PositionCorrection(AttackCol, GetCorrectionPos(2.0f, correctionPos));
		
		if (HelperTimer::IsEasingOver())
		{
			handle = EffekseerManager::StopEffect(handle);
			HelperTimer::ResetTimer();
			weak_player.lock()->ResetAnimation();
			weak_player.lock()->PlayAnimation(0, 0);
			AttackCol->ChangeColFlag();
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
		}

		//ノックバック
		if (Player::BossHIt::IsFlag())
		{
			handle = EffekseerManager::StopEffect(handle);
			AttackCol->ChangeColFlag();
			Player::BossHIt::FalseFlag();
			HelperTimer::ResetTimer();
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_KnockBack>());
		}
	}
}

void PlayerState_Rush::Finalize()
{
}
