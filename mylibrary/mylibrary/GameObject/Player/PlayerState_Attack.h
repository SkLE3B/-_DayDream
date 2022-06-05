#pragma once
#include "PlayerBaseState.h"

enum class AttackStep
{
    AttackStart,
    DuringAttack
};

class PlayerState_Attack :
    public PlayerBaseState
{
public:
    PlayerState_Attack()
    {
        statePattern = PlayerStatepattern::ATTACK;
    }

    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;

private:
    AttackStep step;
    Effekseer::Handle handle;//エフェクトハンドル
};

