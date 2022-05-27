#pragma once
#include "Playerbasestate.h"

enum class NoneStep
{
    NoneStart,
    DuringNone
};

class PlayerState_None :
    public PlayerBaseState
{
public:
    PlayerState_None()
    {
        statePattern = PlayerStatepattern::NONE;
    }

    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;

private:
    NoneStep step;
};