#include "PlayerState_AvoidLeft.h"
#include "PlayerState_None.h"
#include "PlayerState_KnockBack.h"

void PlayerState_AvoidLeft::Initialize()
{
	step = AvoidStepLeft::AvoidStart;
	timer.SetMaxTime(0.7f);
}

void PlayerState_AvoidLeft::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	timer.Update();
	if (step == AvoidStepLeft::AvoidStart)
	{
		weak_player.lock()->PlayAnimation(2, 0);
		audio->PlayWave(L"Resources/sounds/avoid.wav");
		timer.Reset();
		DecisionDistance(weak_player.lock()->GetCameraDirectionX() * -30);
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/movePlayer.efk", {
			weak_player.lock()->GetPosition().x, weak_player.lock()->GetPosition().y + 5,
			weak_player.lock()->GetPosition().z });
		EffekseerManager::SetRotation(handle, { 0,60,0 });
		step = AvoidStepLeft::DuringAvoid;
	}

	if (timer.IsTimer() && step == AvoidStepLeft::DuringAvoid)
	{
		const float avoidFrame = 0.7f;
		Easing(weak_player.lock()->GetPosition(), avoidFrame);

		if (timer.IsEasingOver())
		{
			handle = EffekseerManager::StopEffect(handle);
			weak_player.lock()->ResetAnimation();
			weak_player.lock()->PlayAnimation(0, 0);
	
			if (input->RelesePush(DIK_Q))
			{
				weak_player.lock()->PlayAnimation(0, 0);
			}
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
		}
	}

	if (Player::BossHIt::IsFlag())
	{
		AttackCol->ChangeColFlag();
		Player::BossHIt::FalseFlag();
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_KnockBack>());
	}
}

void PlayerState_AvoidLeft::Finalize()
{
}
