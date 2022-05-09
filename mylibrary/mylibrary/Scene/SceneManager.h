#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"

/// <summary>
/// �V�[���Ǘ�
/// </summary>
class SceneManager
{
public:
	static SceneManager* GetInstance();

	/// <summary>
	/// �X�V
	/// </summary>
	void Update(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
		DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog,PostEffect* post);

	/// <summary>
	/// �`��
	/// </summary>
	void Draw();

	/// <summary>
	/// �t�H�O�`��
	/// </summary>
	void FogDraw();

	/// <summary>
	/// UI�`��1
	/// </summary>
	void UIDraw();

	/// <summary>
	/// �O�ʃX�v���C�g�`��
	/// </summary>
	void BgDraw();

	/// <summary>
	/// �V�[���`�F���W�t���O�擾
	/// </summary>
	/// <returns></returns>
	bool& GetSceneChengeFlag();

	/// <summary>
	/// ���̃V�[���\��
	/// </summary>
	/// <param name="sceneName">�V�[����</param>
	void ChangeScene(const std::string& sceneName);

	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { this->sceneFactory = sceneFactory;}
	void SetSceneState(const Scene& sceneState) { this->sceneState = sceneState; }

	Scene& GetSceneState() { return sceneState; }
private:
	//���̃V�[��
	BaseScene* scene = nullptr;
	//���̃V�[��
	BaseScene* nextScene = nullptr;
	//��ԃV�[��
	Scene sceneState;
	//�V�[���t�@�N�g���[
	AbstractSceneFactory* sceneFactory = nullptr;

	SceneManager()  = default;
	~SceneManager();
	SceneManager(const SceneManager&) = delete;
	void operator = (const SceneManager&) = delete;
};