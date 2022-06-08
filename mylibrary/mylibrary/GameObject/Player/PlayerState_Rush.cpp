#include "PlayerState_Rush.h"
#include "PlayerState_None.h"
#include "PlayerState_KnockBack.h"
#include "PlayerState_RoarKnockBack.h"

void PlayerState_Rush::Initialize()
{
	step = RushStep::RushStart;
	timer.SetMaxTime(2.09f);
}

void PlayerState_Rush::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	timer.Update();
	if (step == RushStep::RushStart)
	{
		weak_player.lock()->PlayAnimation(1, 0);
		audio->PlayWave(L"Resources/sounds/Drill.wav");
		timer.Reset();
		AttackCol->SetPosition(weak_player.lock()->GetPosition());
		DecisionDistance(weak_player.lock()->GetMoveDirection() * 60);
		AttackCol->ChangeColFlag();
		step = RushStep::DuringRush;
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/movePlayer.efk", { weak_player.lock()->GetPosition().x,
			weak_player.lock()->GetPosition().y + 5, weak_player.lock()->GetPosition().z});
	}

	if (timer.IsTimer() && step == RushStep::DuringRush)
	{
		const float AttackFlame2 = 2.09f;
		Vector3 correctionPos = { 0,2,0 };
		Easing(weak_player.lock()->GetPosition(), AttackFlame2);
		PositionCorrection(AttackCol, GetCorrectionPos(2.0f, correctionPos));
		
		if (timer.IsEasingOver())
		{
			handle = EffekseerManager::StopEffect(handle);
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
			AttackCol->SetPosition(weak_player.lock()->GetPosition());
			Player::BossHIt::FalseFlag();
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_KnockBack>());
		}
	}
}

void PlayerState_Rush::Finalize()
{
}
