#pragma once
#include "playerbasestate.h"

enum class RushStep
{
    RushStart,
    DuringRush
};

class PlayerState_Rush :
    public PlayerBaseState
{
public:

    PlayerState_Rush()
    {
        statePattern = PlayerStatepattern::RUSH;
    }

    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;
private:
    RushStep step;
};

