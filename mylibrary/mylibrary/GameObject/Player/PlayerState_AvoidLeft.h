#pragma once
#include "PlayerBaseState.h"

enum class AvoidStepLeft
{
    AvoidStart,
    DuringAvoid
};

class PlayerState_AvoidLeft :
    public PlayerBaseState
{
public:
    PlayerState_AvoidLeft()
    {
        statePattern = PlayerStatepattern::AVOID;
    }

    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;
private:
    Effekseer::Handle handle;//エフェクトハンドル
    AvoidStepLeft step;
    Vector3 cameraDirectionX;
    DirectX::XMMATRIX camMatWorld;
};

