#include "GameScene.h"
#include "EndScene.h"
#include<DirectXMath.h>
#include<windows.h>
#include<DirectXMath.h>
#include<d3dcompiler.h>
#include<string>
#include<DirectXTex.h>
#include <iomanip>
#include<wrl.h>
#include "../DirectXGame.h"
#include "../3d/Object3d.h"
#include "../Collision/SphereCollider.h"
#include "../Collision/MeshCollider.h"
#include "../Collision/CollisionManager.h"
#include "../GameObject/Player/Player.h"
#include "../3d/TouchableObject.h"
#include "../Collision/AttackCollisionObject.h"
#include "../Math/Vector3.h"
#include "../GameObject/Boss/BossBaseState.h"
#include "../Scene/SceneManager.h"
#include "../Base/LevelLoader.h"
#include "../mylibrary/GameObject/Player/PlayerState_None.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

GameScene::GameScene()
{
	scenePattern = Scene::Game;
}

GameScene::~GameScene()
{
	Safe_Delete(levelData);
	Safe_Delete(particleMan);
	Safe_Delete(model_Player);
	Safe_Delete(model_Ground);
	Safe_Delete(model_AttackObject);
	Safe_Delete(model_Boss);
	Safe_Delete(model_EnemyAttackObject);
	Safe_Delete(model_EnemyAttackObject);
}

void GameScene::Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
	DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post)
{
	BaseScene::Initialize(dxCommon, input, audio, tMng, dCamera, bCamera, gamepad, dText,fog,post);

	// �p�[�e�B�N���}�l�[�W������
	particleMan = ParticleManager::Create(dxCommon->GetDevice(), dCamera);
	collisionManager = CollisionManager::GetInstance();
	audio->Stop();
	//�T�E���h�̓ǂݍ���
	audio->PlayLoop(L"Resources/sounds/Game.wav");
	//�X�v���C�g����
	SpriteInitialize(tMng);

	//�f�o�C�X���Z�b�g
	Object3d::SetDevice(dxCommon->GetDevice());
	//�J�������Z�b�g
	Object3d::SetCamera(dCamera);
	//�O���t�B�b�N�X�p�C�v���C������
	Object3d::CreateGraphicsPipeline();

	ObjectObj::SetCamera(dCamera);
	ObjectObj::CreateGraphicsPipeline();

	// �J���������_���Z�b�g
	dCamera->SetTarget({ 0,15,-30 });
	dCamera->SetDistance(25.0f);

	//���x���f�[�^�̓ǂݍ���
	levelData = LevelLoader::LoadFile("GameScene");
	//FBX
	//���f�������w�肵�ăt�@�C���ǂݍ���                  
	model_Player = FbxLoader::GetInstance()->LoadModelFromFile("armar");
	model_Ground = FbxLoader::GetInstance()->LoadModelFromFile("ground");
	model_Boss = FbxLoader::GetInstance()->LoadModelFromFile("boss");
	model_AttackObject = FbxLoader::GetInstance()->LoadModelFromFile("Attack");
	model_EnemyAttackObject = FbxLoader::GetInstance()->LoadModelFromFile("Esphere");

	//3D�I�u�W�F�N�g�����ƃ��f���̃Z�b�g
	//FBX
	object_Player = Player::Create(model_Player,dCamera);
	object_Player->SetPosition({ 0,0,-30 });
	object_Player->SetRotation({ 0,0,0 });
	object_Ground = TouchableObject::Create(model_Ground);
	object_Ground->SetPosition({ 0,0,0 });
	object_Ground->SetScale({ 8,8,8 });
	objectBoss = Boss::Create(model_Boss);
	objectBoss->SetPosition({0,0,30});
	objectBoss->SetRotation({0,180,0});
	objectAttack = AttackCollisionObject::Create(model_AttackObject);
	objectAttack->SetPosition({10,0,10});
	objectAttack->SetScale({ 4,4,4 });
	objectEAttack = AttackEnemyCollisionObject::Create(model_EnemyAttackObject);
	objectEAttack->SetPosition({10,-5,30});
	objectEAttack->SetScale({ 4,4,4 });

	state = "OFF";
	timer = 0;
	sceneChengeFlag = false;
	fog->SetAlpha(0.0f);
	fog->ResetFade(1.0f, fog->GetFadeOutFlag());
	this->fog = fog;
	fog->SetAlpha(0.3f);
	swich = "ON";
}

void GameScene::Update()
{
	if (fog->Morethan(0.3f))
	{
		fog->FadeIn(0.01f);
	}
	
	if (object_Player->GetLifeFlag() && spriteWarning->Lessthan(0.1f))
	{
		object_Player->Update(debugCamera, objectAttack.get(), objectBoss.get(), audio);
	}

	if (spriteWarning->Lessthan(0.1f))
	{
		objectBoss->Update(object_Player.get(), objectEAttack.get(), audio);
		//�J�����X�V
		debugCamera->Update();
	}

	//�Q�[���p�b�h�X�V
	gamepad->Update();
	//�p�[�e�B�N������
	particleMan->Update();
	object_Ground->Update();
	objectAttack->Update(object_Player.get(), particleMan, audio);
	objectEAttack->Update(objectBoss.get(), object_Player.get(), audio);
	collisionManager->CheckAllCollisions();

	if (state == "OFF")
	{
		spriteWarning->Fadein("ON");
	}
	else if (state == "ON")
	{
		spriteWarning->Fadeout("ON");
	}

	if(spriteWarning->isMax())
	{
		state = "ON";
	}

	float bossHp = objectAttack->GetHP();
	float playerHp = object_Player->GetPlayerHP();
	spriteBossHpBack->SetSize(bossHp, 30);
	spritePlayerHpBack->SetSize(playerHp, 30);
	
	if (playerHp == 0)
	{
		EffekseerManager::SetScale(handle, { 5,5,5 });
		handle = EffekseerManager::PlayEffect(u"Resources/Effects/Pdead.efk", { object_Player->GetPosition().x, object_Player->GetPosition().y, object_Player->GetPosition().z });
		GotoEndScene(playerHp, 1);
	}
	if (bossHp == 0)
	{
		GotoEndScene(bossHp, 2);
	}
}

void GameScene::Draw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	const bool DrawFlag = object_Player->GetColFlag();//�����蔻��`��t���O
	const bool DrawBossCollisionFlag = objectBoss->GetColFlag();

	//3D�I�u�W�F�N�g�̕`��
	if (object_Player->GetLifeFlag())
	{
		object_Player->Draw(cmdList);
	}
	
	if (DrawBossCollisionFlag)
	{
		objectEAttack->Draw(cmdList);
	}
	
	object_Ground->Draw(cmdList);
    objectBoss->Draw(cmdList);
}

void GameScene::FogDraw()
{
	//// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	ObjectObj::PreDraw(dxCommon->GetCommandList());
	particleMan->Draw(cmdList);
	ObjectObj::PostDraw();
}

void GameScene::UIDraw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//�X�v���C�g�̃p�C�v���C�����Z�b�g
	Sprite::SetPipelineState(dxCommon->GetCommandList());
	// �O�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	spriteBossHpBack->Draw(dxCommon->GetCommandList());
	spriteBossHpFront->Draw(dxCommon->GetCommandList());
	spritePlayerHpBack->Draw(dxCommon->GetCommandList());
	spritePlayerHpFront->Draw(dxCommon->GetCommandList());
	spriteWarning->Draw(dxCommon->GetCommandList());
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
}

void GameScene::BgDraw()
{
	// �R�}���h���X�g�̎擾
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//�X�v���C�g�̃p�C�v���C�����Z�b�g
	Sprite::SetPipelineState(dxCommon->GetCommandList());
	// �w�i�X�v���C�g�`��O����
	Sprite::PreDraw(cmdList);
	// �X�v���C�g�`��㏈��
	Sprite::PostDraw();
	// �[�x�o�b�t�@�N���A
	dxCommon->ClearRenderTarget();
}

void GameScene::CreateParticles()
{
	for (int i = 0; i < 10; i++) {
		// X,Y,Z�S��[-5.0f,+5.0f]�Ń����_���ɕ��z
		const float rnd_pos = 30.0f;
		XMFLOAT3 pos{};
		pos.x = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.y = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;
		pos.z = (float)rand() / RAND_MAX * rnd_pos - rnd_pos / 2.0f;

		const float rnd_vel = 0.1f;
		XMFLOAT3 vel{};
		vel.x = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.y = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;
		vel.z = (float)rand() / RAND_MAX * rnd_vel - rnd_vel / 2.0f;

		XMFLOAT3 acc{};
		const float rnd_acc = 0.001f;
		acc.y = -(float)rand() / RAND_MAX * rnd_acc;

		// �ǉ�
		particleMan->Add(60, pos, vel, acc, 3.0f, 0.0f);
	}
}

void GameScene::GotoEndScene(float Hp,int dead)
{
	const int playerDead = 1;
	const int bossDead = 2;
	const int endTime = 400;

	if (Hp == 0 && object_Player->GetLifeFlag() && dead == playerDead)
	{
		object_Player->GetLifeFlag() = !object_Player->GetLifeFlag();
	}

	if (!object_Player->GetLifeFlag())
	{
		timer++;
	
		if (timer >= 25)
		{
			handle = EffekseerManager::StopEffect(handle);
		}

		if (timer >= endTime)
		{
			timer = 0;
			object_Player->ChangeState(std::make_shared<PlayerState_None>());
			//�G���h�V�[����
			SceneManager::GetInstance()->ChangeScene("END");
			SceneManager::GetInstance()->SetSceneState(Scene::End);
		}
	}

	if (Hp == 0 && objectBoss->GetLifeFlag() && dead == bossDead)
	{
		objectBoss->PlayAnimation(4, 1);
		objectBoss->ChangeLifeFlag();
		audio->PlayWave(L"Resources/sounds/Dead.wav");
	}

	if (!objectBoss->GetLifeFlag())
	{
		timer++;
		
		if (timer >= endTime)
		{
			timer = 0;
			//�G���h�V�[����
			SceneManager::GetInstance()->SetSceneState(Scene::End);
			SceneManager::GetInstance()->ChangeScene("END");
		}
	}
}

void GameScene::LoadTexture(TextureManager* textureManager)
{
	tMng->spriteLoadTexture(0, L"Resources/textures/background.png");
	tMng->spriteLoadTexture(1, L"Resources/textures/playerHpFront2.png");
	tMng->spriteLoadTexture(3, L"Resources/textures/bossHpFront2.png");
	tMng->spriteLoadTexture(4, L"Resources/textures/bossHpBack.png");
	tMng->spriteLoadTexture(5, L"Resources/textures/playerHpBack.png");
	tMng->spriteLoadTexture(6, L"Resources/textures/Warning.png");
}

void GameScene::ObjectInitialize(LevelData::ObjectData objectData, Object3d* object)
{
	// ���W
	DirectX::XMFLOAT3 pos;
	DirectX::XMStoreFloat3(&pos, objectData.translation);
	object->SetPosition(pos);

	// ��]�p
	DirectX::XMFLOAT3 rot;
	DirectX::XMStoreFloat3(&rot, objectData.rotation);
	object->SetRotation(rot);

	// ���W
	DirectX::XMFLOAT3 scale;
	DirectX::XMStoreFloat3(&scale, objectData.scaling);
	object->SetScale(scale);
}

void GameScene::SetFile()
{

}

void GameScene::SpriteInitialize(TextureManager* textureManager)
{
	//�e�N�X�`���ǂݍ���
	LoadTexture(textureManager);

	spriteBackGround = make_unique<Sprite>();
	spriteBackGround->Initialize(0);
	spriteBackGround->SetPosition(0, 0);
	spriteBackGround->SetSize(1280, 1080);
	spriteBossHpFront = make_unique<Sprite>();
	spriteBossHpFront->Initialize(3);
	spriteBossHpFront->SetSize(1000, 35);
	spriteBossHpFront->SetPosition(0, -1);
	spriteBossHpBack = make_unique<Sprite>();
	spriteBossHpBack->Initialize(4);
	spriteBossHpBack->SetPosition(0, 0);
	spriteBossHpBack->SetSize(1000, 30);
	spritePlayerHpFront = make_unique<Sprite>();
	spritePlayerHpFront->Initialize(1);
	spritePlayerHpFront->SetPosition(480, 690);
	spritePlayerHpFront->SetSize(800, 30);
	spritePlayerHpBack = make_unique<Sprite>();
	spritePlayerHpBack->Initialize(5);
	spritePlayerHpBack->SetPosition(480, 690);
	spritePlayerHpBack->SetSize(800, 30);
	spriteWarning = make_unique<Sprite>();
	spriteWarning->Initialize(6);
	spriteWarning->SetAlpha(0);
	spriteWarning->SetPosition(350, 50);
	spriteWarning->SetSize(600, 300);
}