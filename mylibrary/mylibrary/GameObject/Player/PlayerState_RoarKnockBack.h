#pragma once
#include "PlayerBaseState.h"

enum class RoarKnockBackStep
{
    RoarKnockBackStart,
    RoarDuringKnockBack
};

class PlayerState_RoarKnockBack :
    public PlayerBaseState
{
public:
    PlayerState_RoarKnockBack()
    {
        statePattern = PlayerStatepattern::ROARKNOCKBACK;
    }

    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;

private:
    RoarKnockBackStep step;
};

