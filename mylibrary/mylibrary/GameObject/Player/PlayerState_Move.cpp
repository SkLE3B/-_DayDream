#include "PlayerState_Move.h"
#include "PlayerState_None.h"
#include "PlayerState_AvoidRight.h"
#include "PlayerState_AvoidLeft.h"
#include "PlayerState_Attack.h"
#include "PlayerState_Rush.h"
#include "PlayerState_KnockBack.h"
#include "PlayerState_RoarKnockBack.h"

void PlayerStateMove::Initialize()
{
	moveDirection = 0;
	cameraDirectionX = Vector3(camMatWorld.r[0].m128_f32[0], 0, camMatWorld.r[0].m128_f32[2]).Normalize();
	cameraDirectionZ = Vector3(camMatWorld.r[2].m128_f32[0], 0, camMatWorld.r[2].m128_f32[2]).Normalize();
	moveSpeed = 0.55f;
}

void PlayerStateMove::Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio)
{
	//�ړ��x�N�g�����K��
	cameraDirectionX.Normalize();
	cameraDirectionZ.Normalize();

	//�m�b�N�o�b�N
	if (Player::BossHIt::IsFlag())
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_KnockBack>());
	}

	//���ړ�
	if (input->IsPush(DIK_A)) {
		Move(weak_player.lock()->GetCameraDirectionX() * -1);
	}
	//�E�ړ�
	if (input->IsPush(DIK_D)) {
		Move(weak_player.lock()->GetCameraDirectionX());
	}
	//��ړ�
	if (input->IsPush(DIK_W)) {
		Move(weak_player.lock()->GetCameraDirectionZ());
	}
	//���ړ�
	if (input->IsPush(DIK_S)) {
		Move(weak_player.lock()->GetCameraDirectionZ() * -1);
	}
	//�ړ�������
	if (input->RelesePush(DIK_A) || input->RelesePush(DIK_D) || input->RelesePush(DIK_W) || input->RelesePush(DIK_S))
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
	}
	//�E�ɔ�����
    else if (input->TriggerPush(DIK_E))
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_AvoidRight>());
	}
	//���ɔ�����
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

void PlayerStateMove::Finalize()
{
}

void PlayerStateMove::Move(Vector3 cameraDirection)
{
	weak_player.lock()->GetPosition() += cameraDirection * moveSpeed;
}
