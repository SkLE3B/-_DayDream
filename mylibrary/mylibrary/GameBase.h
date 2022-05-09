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
/// �Q�[���̋��ʊ��
/// </summary>
class GameBase
{
public:
	/// <summary>
	/// ���s
	/// </summary>
	virtual void Run();

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize();

	/// <summary>
	/// ���t���[������
	/// </summary>
	/// <returns>
	/// �I���t���O�@true : �I��false : �p��
	/// </returns>
	virtual void Update();

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;

	/// <summary>
	/// ��n��
	/// </summary>
	virtual void Finalize();

protected:
	//�I���t���O
	bool endFlag = false;
	//�E�B���h�E�Y�A�v��
	std::unique_ptr<windowsApi> winApp;
	//�e�N�X�`���}�l�[�W���[
	std::unique_ptr<TextureManager> textureManager;
	//�I�[�f�B�I�}�l�[�W���[
	std::unique_ptr<AudioManager> audioManger;
	//�f�o�b�O�e�L�X�g
	DebugText* debugText;
	//�􉽊w���f��
	std::unique_ptr<GeometoryModel> model;
	//DirectX���
	std::unique_ptr<Direcx12Base> dxBase;
	//�C���v�b�g
	Input* input;
	//�J����
	std::unique_ptr<Camera> camera;
	//�f�o�b�O�J����
	std::unique_ptr<DebugCamera> Dcamera;
	//�f�o�b�O�J����
	std::unique_ptr<BackCamera> Bcamera;
	//�Q�[���p�b�h
	std::unique_ptr<Gamepad> gamepad;
	//�|�X�g�G�t�F�N�g
	PostEffect* postEffect = nullptr;
	//�e�X�g�t�H�O
	std::unique_ptr<Fog> testfog;
	//�t���[�����[�g
	std::unique_ptr<FlameRate> flamerate;
	//�V�[���t�@�N�g���[
	AbstractSceneFactory* sceneFactory = nullptr;
};