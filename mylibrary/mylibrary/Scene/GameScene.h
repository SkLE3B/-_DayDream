#pragma once
#include "BaseScene.h"
#include <DirectXMath.h>
#include "../base/SafeDelete.h"
#include "../3d/EffekseerManager.h"

class CollisionManager;
class Player;
class TouchableObject;
class AttackCollisionObject;
class Boss;
class Fog;

class GameScene :
    public BaseScene
{
private: // �G�C���A�X
// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // �ÓI�����o�ϐ�
	static const int texNumber = 0;
public:
	/// <summary>
    /// �R���X�g�N���^
    /// </summary>
	GameScene();
	/// <summary>
	/// �f�X�g���N�^
	/// </summary>
	~GameScene();

	/// <summary>
	/// ������
	/// </summary>
	void Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
	DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post) override;

	/// <summary>
	/// ���t���[������
	/// </summary>
	void Update()  override;

	/// <summary>
	/// �`��
	/// </summary>
	void Draw()    override;

	void FogDraw() override;

	/// <summary>
	/// Imgui�`��
	/// </summary>
	void UIDraw()  override;

	/// <summary>
	/// �w�i�`��
	/// </summary>
	void BgDraw()  override;

    /// <summary>
    /// �p�[�e�B�N������
    /// </summary>
    void CreateParticles();

	/// <summary>
	/// �G���h�V�[���Ɉړ�
	/// </summary>
	/// <param name="Hp">HP</param>
	/// <param name="dead">���񂾃I�u�W�F�N�g(1�Ȃ�v���C���[2�Ȃ�{�X)</param>
	void GotoEndScene(float Hp,int dead);
private:
	//�X�v���C�g�p
	std::unique_ptr<Sprite> sprite;
	std::unique_ptr<Sprite> sprite2;
	std::unique_ptr<Sprite> sprite3;
	std::unique_ptr<Sprite> sprite4;
	std::unique_ptr<Sprite> sprite5;
	std::unique_ptr<Sprite> spriteWarning;
	ParticleManager* particleMan = nullptr;
	Fog* fog = nullptr;
	std::string state;

	//�I�u�W�F�N�g�p<FBX>
	Model* model_Player            = nullptr;
	Model* model_Ground            = nullptr;
	Model* model_Boss              = nullptr;
	Model* model_AttackObject      = nullptr;
	Model* model_EnemyAttackObject = nullptr;

	std::shared_ptr<Player> object_Player;
	TouchableObject*        object_Ground = nullptr;
	std::shared_ptr<Boss>   objectBoss;

	std::shared_ptr<AttackCollisionObject> objectAttack;
	std::shared_ptr<AttackEnemyCollisionObject> objectEAttack;

	Effekseer::Handle handle;

	//�I�u�W�F�N�g<OBJ>
	ObjLoder* modelcube = nullptr;
	ObjectObj* objcube = nullptr;
	CollisionManager* collisionManager = nullptr;
	
	float pPos[3]      = { 0,0,0 };
	float Bpos[3]      = { 0,0,0 };
	float BApos[3]     = { 0,0,0 };
	float Bros[3]      = { 0,0,0 };
	float rot[3]       = { 0,0,0 };
	float Time[3]      = { 0,0,0 };
	float Timerate[3]  = { 0,0,0 };
	float BTime[3]     = { 0,0,0 };
	float BTimerate[3] = { 0,0,0 };
	int   HP[3]        = { 0,0,0 };
	float Angle[3]     = {};
	float vectorPos[3] = {};
	float Stamina[3]   = {0,0,0};
	float PHp[3]       = { 0,0,0 };
	float dotVec[3]    = { 0,0,0 };
	float viewAngle[3] = { 0,0,0 };
	bool  colflag;
	bool  timerflag;
	int   timer;
	int   fadeOutStart;
	int   GoEndScene;
};