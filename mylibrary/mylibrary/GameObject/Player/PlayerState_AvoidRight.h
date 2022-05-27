#pragma once
#include "PlayerBaseState.h"

enum class AvoidStepRight
{
    AvoidStart,
    DuringAvoid
};

class PlayerState_AvoidRight :
    public PlayerBaseState
{
public:
    PlayerState_AvoidRight()
    {
        statePattern = PlayerStatepattern::AVOID;
    }

    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;
private:
    Effekseer::Handle handle;//エフェクトハンドル
    AvoidStepRight step;
    Vector3 cameraDirectionX;
    DirectX::XMMATRIX camMatWorld;
};

