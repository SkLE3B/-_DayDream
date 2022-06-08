#include "PlayerState_Attack.h"
#include "PlayerState_None.h"
#include "PlayerState_KnockBack.h"
#include "../mylibrary/Base/HelperTimer.h"

void PlayerState_Attack::Initialize()
{
	step = AttackStep::AttackStart;
	moveDirection = 0;
	timer.SetMaxTime(1.6f);
}

void PlayerState_Attack::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	timer.Update();
	if (step == AttackStep::AttackStart)
	{
		weak_player.lock()->PlayAnimation(4, 0);
		audio->PlayWave(L"Resources/sounds/swing1.wav");
		timer.Reset();
		Vector3 correctionPos = { 0,2,0 };
		PositionCorrection(AttackCol, GetCorrectionPos(6.0f, correctionPos));
		DecisionDistance(weak_player.lock()->GetMoveDirection() * 30);
		AttackCol->ChangeColFlag();
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/gen.efk", {
			weak_player.lock()->GetPosition().x - (weak_player.lock()->GetCameraDirectionX().Normalize().x), weak_player.lock()->GetPosition().y + 5,
			weak_player.lock()->GetPosition().z });
		EffekseerManager::SetScale(handle,{3,3,3});
		step = AttackStep::DuringAttack;
	}	

	if (timer.IsTimer() && step == AttackStep::DuringAttack)
	{
		const float AttackFlame3 = 1.6f;
		Easing(AttackCol->GetPosition(), AttackFlame3);
		AttackCol->SetPosition(AttackCol->GetPosition());
		
		if (timer.IsEasingOver())
		{
			handle = EffekseerManager::StopEffect(handle);
			weak_player.lock()->ResetAnimation();
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
