#pragma once
#include "Object3d.h"
#include "GameObject/Boss/Boss.h"
#include "Input/Input.h"
#include "player.h"
#include "Audio/AudioManager.h"

class Player;
class Boss;
class AttackEnemyCollisionObject :
    public Object3d
{
public:
	/// <summary>
	/// �I�u�W�F�N�g����
	/// </summary>
	/// <returns></returns>
	static std::shared_ptr<AttackEnemyCollisionObject> Create(Model* model = nullptr);

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize(Model* model);

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="player">�v���C���[�C���X�^���X</param>
	void Update(Boss* boss,Player* player, AudioManager* audio);

private:
	Input* input = Input::GetInstance();
	bool collisionFlag;
	bool roarFlag;
};

