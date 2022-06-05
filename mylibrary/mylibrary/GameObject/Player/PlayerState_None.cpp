#include "PlayerState_None.h"
#include "PlayerState_Move.h"
#include "PlayerState_Attack.h"
#include "PlayerState_AvoidRight.h"
#include "PlayerState_AvoidLeft.h"
#include "PlayerState_Rush.h"
#include "PlayerState_KnockBack.h"
#include "PlayerState_RoarKnockBack.h"

void PlayerState_None::Initialize()
{
	step = NoneStep::NoneStart;
}

void PlayerState_None::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	if (step == NoneStep::NoneStart)
	{
		weak_player.lock()->PlayAnimation(0, 0);
		boss->GetRoarFlag() = false;
		AttackCol->GetColFlag() = false;
		Player::BossHIt::FlagInit();
		step = NoneStep::DuringNone;
	}
	
	if (step == NoneStep::DuringNone)
	{
		//HP��0�ɂȂ�����Q�[���I�[�o�[
		if (weak_player.lock()->GetHp()->isEmpty())
		{
			weak_player.lock()->PlayerDead();
		}

		//�m�b�N�o�b�N
		if (Player::BossHIt::IsFlag())
		{
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_KnockBack>());
		}

		//�ړ�
		if (input->IsPush(DIK_A) || input->IsPush(DIK_D) || input->IsPush(DIK_W) || input->IsPush(DIK_S))
		{
			weak_player.lock()->ChangeState(std::make_shared<PlayerStateMove>());
		}
		//������
		else if (input->TriggerPush(DIK_E))
		{
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_AvoidRight>());
		}
		else if (input->TriggerPush(DIK_Q))
		{
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_AvoidLeft>());
		}
		//�U��
		else if (input->TriggerPush(DIK_DOWN))
		{
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_Attack>());
		}
		//�ːi
		else if (input->TriggerPush(DIK_UP))
		{
			weak_player.lock()->ChangeState(std::make_shared<PlayerState_Rush>());
		}
	}
}

void PlayerState_None::Finalize()
{
}
