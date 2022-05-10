#pragma once
#include "../Base/Direcx12Base.h"
#include "../Input/Gamepad.h"
#include "../2d/DebugText.h"
#include "../2d/Sprite.h"
#include "../Camera/DebugCamera.h"
#include "../PostEffect/PostEffect.h"
#include "../PostEffect/Fog.h"
#include "../Camera/Backcamera.h"
#include "../3d/ParticleManager.h"
#include "../PostEffect/PostEffect.h"
#include "../3d/ObjectObj.h"
#include "../3d/ObjLoder.h"
#include "../3d/FbxLoader.h"
#include "../3d/Object3d.h"
#include "../Audio/AudioManager.h"

//ëOï˚êÈåæ
class SceneManager;

enum class Scene
{
    Title,
	Game,
	End
};

class BaseScene
{
public:
	BaseScene();

	virtual ~BaseScene() = default;

	virtual void Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
		DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText, Fog* fog, PostEffect* post);

	virtual void Update()  = 0;

	virtual void Draw()    = 0;

	virtual void UIDraw()  = 0;

	virtual void BgDraw()  = 0;

	virtual void FogDraw() = 0;

	BaseScene* GetNextScene() { return nextScene; }
	Scene GetScene() { return scenePattern; }
	bool& GetSceneChengeFlag() { return sceneChengeFlag; }
	bool& ChangeSceneChengeFlag() { return sceneChengeFlag = !sceneChengeFlag; }
protected:
	Direcx12Base*   dxCommon    = nullptr;
	Input*          input       = nullptr;
	AudioManager*   audio       = nullptr;
	BaseScene*      nextScene   = nullptr;
	TextureManager* tMng        = nullptr;
	DebugCamera*    debugCamera = nullptr;
	BackCamera*     backCamera  = nullptr;
	Gamepad*        gamepad     = nullptr;
	DebugText*      dText       = nullptr;
	PostEffect*     post        = nullptr;
	Fog*            fog         = nullptr;
	Scene           scenePattern;
	bool            sceneChengeFlag;
	bool            sceneEnd;
};