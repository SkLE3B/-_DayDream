#pragma once
#include "BaseScene.h"
#include <DirectXMath.h>
#include "../base/SafeDelete.h"
#include "../3d/EffekseerManager.h"
#include "../Base/LevelLoader.h"
#include <map>

struct LevelData;

class CollisionManager;
class Player;
class TouchableObject;
class AttackCollisionObject;
class Boss;
class Fog;

class GameScene :
    public BaseScene
{
private: // エイリアス
// Microsoft::WRL::を省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	// DirectX::を省略
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMVECTOR = DirectX::XMVECTOR;
	using XMMATRIX = DirectX::XMMATRIX;

private: // 静的メンバ変数
	static const int texNumber = 0;
public:
	/// <summary>
    /// コンストクラタ
    /// </summary>
	GameScene();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GameScene();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
	DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post) override;

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update()  override;

	/// <summary>
	/// 描画
	/// </summary>
	void Draw()    override;

	void FogDraw() override;

	/// <summary>
	/// Imgui描画
	/// </summary>
	void UIDraw()  override;

	/// <summary>
	/// 背景描画
	/// </summary>
	void BgDraw()  override;

    /// <summary>
    /// パーティクル生成
    /// </summary>
    void CreateParticles();

	/// <summary>
	/// エンドシーンに移動
	/// </summary>
	/// <param name="Hp">HP</param>
	/// <param name="dead">死んだ対象オブジェクト(1ならプレイヤー2ならボス)</param>
	void GotoEndScene(float Hp,int dead);

	/// <summary>
	/// スプライト初期化
	/// </summary>
	void SpriteInitialize(TextureManager* textureManager);

	/// <summary>
	///	テクスチャ読み込み
	/// </summary>
	void LoadTexture(TextureManager* textureManager);

	void ObjectInitialize(LevelData::ObjectData objectData, Object3d* object);

	void SetFile();

private:
	LevelData* levelData = nullptr;
	//スプライト用
	std::unique_ptr<Sprite> spriteBackGround;
	std::unique_ptr<Sprite> spriteBossHpFront;
	std::unique_ptr<Sprite> spriteBossHpBack;
	std::unique_ptr<Sprite> spritePlayerHpFront;
	std::unique_ptr<Sprite> spritePlayerHpBack;
	std::unique_ptr<Sprite> spriteWarning;
	ParticleManager* particleMan = nullptr;
	Fog* fog = nullptr;
	std::string state;

	//オブジェクト用<FBX>
	Model* model_Player            = nullptr;
	Model* model_Ground            = nullptr;
	Model* model_Boss              = nullptr;
	Model* model_AttackObject      = nullptr;
	Model* model_EnemyAttackObject = nullptr;

	//TouchableObject* object_Ground              = nullptr;
	//Player* object_Player                       = nullptr;
	//Boss* objectBoss                            = nullptr;
	//AttackCollisionObject* objectAttack         = nullptr;
    //AttackEnemyCollisionObject * objectEAttack  = nullptr;
	
	std::shared_ptr<Player> object_Player;
	std::shared_ptr<Boss>   objectBoss;
	std::shared_ptr<AttackCollisionObject> objectAttack;
	std::shared_ptr<AttackEnemyCollisionObject> objectEAttack;
	std::shared_ptr<TouchableObject> object_Ground;

	std::map<std::string, Model*> models;
	std::vector<std::shared_ptr<Object3d>> objects;
	CollisionManager* collisionManager = nullptr;
	Effekseer::Handle handle;

	std::string swich;

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