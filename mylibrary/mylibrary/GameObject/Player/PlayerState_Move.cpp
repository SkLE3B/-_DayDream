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
	//移動ベクトル正規化
	cameraDirectionX.Normalize();
	cameraDirectionZ.Normalize();

	//ノックバック
	if (Player::BossHIt::IsFlag())
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_KnockBack>());
	}

	//左移動
	if (input->IsPush(DIK_A)) {
		Move(weak_player.lock()->GetCameraDirectionX() * -1);
	}
	//右移動
	if (input->IsPush(DIK_D)) {
		Move(weak_player.lock()->GetCameraDirectionX());
	}
	//上移動
	if (input->IsPush(DIK_W)) {
		Move(weak_player.lock()->GetCameraDirectionZ());
	}
	//下移動
	if (input->IsPush(DIK_S)) {
		Move(weak_player.lock()->GetCameraDirectionZ() * -1);
	}
	//移動した後
	if (input->RelesePush(DIK_A) || input->RelesePush(DIK_D) || input->RelesePush(DIK_W) || input->RelesePush(DIK_S))
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_None>());
	}
	//右に避ける
    else if (input->TriggerPush(DIK_E))
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_AvoidRight>());
	}
	//左に避ける
	else if (input->TriggerPush(DIK_Q))
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_AvoidLeft>());
	}
	//攻撃
	else if (input->TriggerPush(DIK_DOWN))
	{
		weak_player.lock()->ChangeState(std::make_shared<PlayerState_Attack>());
	}
	//突進
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
