#pragma once
#include "../3d/Object3d.h"
#include "../3d/ParticleManager.h"
#include "../player.h"
#include "../Math/ExNum.h"
#include "../Audio/AudioManager.h"
#include "../3d/EffekseerManager.h"

class Player;
class ParticleManager;
class AttackCollisionObject :
    public Object3d
{
public:
	/// <summary>
    /// �I�u�W�F�N�g����
    /// </summary>
    /// <returns></returns>
	static std::shared_ptr<AttackCollisionObject> Create(Model* model = nullptr, Camera* camera = nullptr);

	/// <summary>
	/// ������
	/// </summary>
	/// <returns>����</returns>
	bool Initialize(Model* model, Camera* camera);

	/// <summary>
	/// �X�V
	/// </summary>
	/// <param name="player">�v���C���[�C���X�^���X</param>
	void Update(Player* player, ParticleManager* particleMan, AudioManager* audio);

	/// <summary>
    /// �p�[�e�B�N������
    /// </summary>
	void CreateParticles(ParticleManager* particleMan);

	/// <summary>
    /// �p�[�e�B�N������
    /// </summary>
	void CreateParticlesAttack(ParticleManager* particleMan);

	bool& GetColFlag() { return collisionFlag; }
	float GetHP() { return HP->rate() * 1000; }
	bool& ChangeColFlag() { return collisionFlag = !collisionFlag; }

public:
	static AttackCollisionObject* GetInstance();
private:
	bool collisionFlag;
	bool soundFlag;
	int soundTimer;
	ExNum* HP;
	Effekseer::Handle handle;
};

