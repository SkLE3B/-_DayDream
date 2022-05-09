#pragma once
#include "Base/windowsApi.h"
#include "Input/Input.h"
#include "Base/Direcx12Base.h"
#include "3d/GeometoryModel.h"
#include "2d/TextureManager.h"
#include "2d/DebugText.h"
#include "Audio/AudioManager.h"
#include "3d/GeometoryObject.h"
#include "2d/Sprite.h"
#include "3d/FbxLoader.h"
#include "3d/Object3d.h"
#include "Camera/DebugCamera.h"
#include "Input/Gamepad.h"
#include "3d/ParticleManager.h"
#include "PostEffect/PostEffect.h"
#include "3d/ObjectObj.h"
#include "3d/ObjLoder.h"
#include "Base/FlameRate.h"
#include "Collision/AttackCollisionObject.h"
#include "3d/AttackEnemyCollisionObject.h"
#include "PostEffect/Fog.h"
#include "Scene/AbstractSceneFactory.h"
#include "3d/EffekseerManager.h"
#include<memory>

#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib,"dinput8.lib")

/// <summary>
/// ゲームの共通基盤
/// </summary>
class GameBase
{
public:
	/// <summary>
	/// 実行
	/// </summary>
	virtual void Run();

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	/// <returns>
	/// 終了フラグ　true : 終了false : 継続
	/// </returns>
	virtual void Update();

	/// <summary>
	/// 描画
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// 後始末
	/// </summary>
	virtual void Finalize();

protected:
	//終了フラグ
	bool endFlag = false;
	//ウィンドウズアプリ
	std::unique_ptr<windowsApi> winApp;
	//テクスチャマネージャー
	std::unique_ptr<TextureManager> textureManager;
	//オーディオマネージャー
	std::unique_ptr<AudioManager> audioManger;
	//デバッグテキスト
	DebugText* debugText;
	//幾何学モデル
	std::unique_ptr<GeometoryModel> model;
	//DirectX基盤
	std::unique_ptr<Direcx12Base> dxBase;
	//インプット
	Input* input;
	//カメラ
	std::unique_ptr<Camera> camera;
	//デバッグカメラ
	std::unique_ptr<DebugCamera> Dcamera;
	//デバッグカメラ
	std::unique_ptr<BackCamera> Bcamera;
	//ゲームパッド
	std::unique_ptr<Gamepad> gamepad;
	//ポストエフェクト
	PostEffect* postEffect = nullptr;
	//テストフォグ
	std::unique_ptr<Fog> testfog;
	//フレームレート
	std::unique_ptr<FlameRate> flamerate;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory = nullptr;
};