#pragma once
#include "playerbasestate.h"

enum class KnockBackStep
{
    KnockBackStart,
    DuringKnockBack
};

class PlayerState_KnockBack :
    public PlayerBaseState
{
public:
    PlayerState_KnockBack()
    {
        statePattern = PlayerStatepattern::KNOCKBACK;
    }
  
    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;
private:
    KnockBackStep step;
};

