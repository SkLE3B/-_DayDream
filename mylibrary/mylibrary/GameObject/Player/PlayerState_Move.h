#pragma once
#include "PlayerBaseState.h"

class PlayerStateMove :
    public PlayerBaseState
{
public:
    PlayerStateMove()
    {
        statePattern = PlayerStatepattern::MOVE;
    }

    void Initialize() override;

    void Update(Camera* camera, AttackCollisionObject* AttackCol, Boss* boss, AudioManager* audio) override;

    void Finalize() override;
    
    void Move(Vector3 cameraDirection);

private:
    Vector3 moveDirection;
    Vector3 cameraDirectionX;
    Vector3 cameraDirectionZ;
    DirectX::XMMATRIX camMatWorld;
    float moveSpeed;
};

