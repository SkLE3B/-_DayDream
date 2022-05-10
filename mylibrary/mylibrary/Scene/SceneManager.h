#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// シーン管理
/// </summary>
class SceneManager
{
public:
	static SceneManager* GetInstance();

	/// <summary>
	/// 更新
	/// </summary>
	void Update(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
		DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog,PostEffect* post);

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	/// <summary>
	/// フォグ描画
	/// </summary>
	void FogDraw();

	/// <summary>
	/// UI描画1
	/// </summary>
	void UIDraw();

	/// <summary>
	/// 前面スプライト描画
	/// </summary>
	void BgDraw();

	/// <summary>
	/// シーンチェンジフラグ取得
	/// </summary>
	/// <returns></returns>
	bool& GetSceneChengeFlag();

	/// <summary>
	/// 次のシーン予約
	/// </summary>
	/// <param name="sceneName">シーン名</param>
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory = sceneFactory;}
	void SetSceneState(const Scene& sceneState) { this->sceneState = sceneState; }

	Scene& GetSceneState() { return sceneState; }
private:
	//今のシーン
	BaseScene* scene = nullptr;
	//次のシーン
	BaseScene* nextScene = nullptr;
	//状態シーン
	Scene sceneState;
	//シーンファクトリー
	AbstractSceneFactory* sceneFactory = nullptr;

	SceneManager()  = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	void operator = (const SceneManager&) = delete;
};