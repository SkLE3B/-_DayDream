#include "SceneManager.h"

SceneManager::~SceneManager()
{
	//delete scene;
}

SceneManager* SceneManager::GetInstance()
{
	static SceneManager instance;

	return &instance;
}

void SceneManager::Update(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
	DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post)
{
	//ƒV[ƒ“Ø‚è‘Ö‚¦‚ª‚ ‚é‚È‚ç
	if (nextScene){
		if (scene){
			delete scene;
		}
		scene = nextScene;
		nextScene = nullptr;

		scene->Initialize(dxCommon,input,audio,tMng,dCamera,bCamera,gamepad,dText,fog,post);
	}

	scene->Update();
}

void SceneManager::Draw()
{
	scene->Draw();
}

void SceneManager::FogDraw()
{
	scene->FogDraw();
}

void SceneManager::UIDraw()
{
	scene->UIDraw();
}

void SceneManager::BgDraw()
{
	scene->BgDraw();
}

bool& SceneManager::GetSceneChengeFlag()
{
	bool SceneChenge;
	SceneChenge = scene->GetSceneChengeFlag();

	return SceneChenge;
}

void SceneManager::ChangeScene(const std::string& sceneName)
{
	assert(sceneFactory);
	assert(nextScene == nullptr);

	nextScene = sceneFactory->CreateScene(sceneName);
}
