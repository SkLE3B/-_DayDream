#include "GameScene.h"
#include "EndScene.h"
#include<DirectXMath.h>
#include<windows.h>
#include<DirectXMath.h>
#include<d3dcompiler.h>
#include<string>
#include<DirectXTex.h>
#include<wrl.h>
#include "../DirectXGame.h"
#include "../3d/Object3d.h"
#include "../Collision/SphereCollider.h"
#include "../Collision/MeshCollider.h"
#include "../Collision/CollisionManager.h"
#include "../Player.h"
#include "../3d/TouchableObject.h"
#include "../Collision/AttackCollisionObject.h"
#include "../Math/Vector3.h"
#include "../GameObject/Boss/BossBaseState.h"
#include "../Scene/SceneManager.h"

using namespace DirectX;
using namespace Microsoft::WRL;
using namespace std;

GameScene::GameScene()
{
	scenePattern = Scene::Game;
}

GameScene::~GameScene()
{
	Safe_Delete(particleMan);
	Safe_Delete(model_Player);
	Safe_Delete(model_Ground);
	Safe_Delete(model_AttackObject);
	Safe_Delete(model_Boss);
	Safe_Delete(model_EnemyAttackObject);
	Safe_Delete(model_EnemyAttackObject);
	Safe_Delete(modelcube);
	Safe_Delete(object_Ground);
	Safe_Delete(objcube);
}

void GameScene::Initialize(Direcx12Base* dxCommon, Input* input, AudioManager* audio, TextureManager* tMng,
	DebugCamera* dCamera, BackCamera* bCamera, Gamepad* gamepad, DebugText* dText,Fog* fog, PostEffect* post)
{
	BaseScene::Initialize(dxCommon, input, audio, tMng, dCamera, bCamera, gamepad, dText,fog,post);

	collisionManager = CollisionManager::GetInstance();
	audio->Stop();
	//サウンドの読み込み
	audio->PlayLoop(L"Resources/sounds/Game.wav");
	//テクスチャ読み込み
	tMng->spriteLoadTexture(0, L"Resources/textures/background.png");
	tMng->spriteLoadTexture(1, L"Resources/textures/HP.png");
	tMng->spriteLoadTexture(3, L"Resources/textures/bossHp.png");
	tMng->spriteLoadTexture(4, L"Resources/textures/HP2.png");
	tMng->spriteLoadTexture(5, L"Resources/textures/HP2.png");
	tMng->spriteLoadTexture(6, L"Resources/textures/Warning.png");

	//スプライト生成
	sprite = make_unique<Sprite>();
	sprite->Initialize(0);
	sprite->SetPosition(0, 0);
	sprite->SetSize(1280, 1080);
	sprite2 = make_unique<Sprite>();
	sprite2->Initialize(3);
	sprite2->SetPosition(0,0);
	sprite3 = make_unique<Sprite>();
	sprite3->Initialize(4);
	sprite3->SetPosition(0, 0);
	sprite3->SetSize(1000, 30);
	sprite4 = make_unique<Sprite>();
	sprite4->Initialize(1);
	sprite4->SetPosition(480, 690);
	sprite4->SetSize(800, 30);
	sprite5 = make_unique<Sprite>();
	sprite5->Initialize(5);
	sprite5->SetPosition(480, 690);
	sprite5->SetSize(800, 30);
	spriteWarning = make_unique<Sprite>();
	spriteWarning->Initialize(6);
	spriteWarning->SetAlpha(0);
	spriteWarning->SetPosition(350,50);
	spriteWarning->SetSize(600, 300);
	
	// パーティクルマネージャ生成
	particleMan = ParticleManager::Create(dxCommon->GetDevice(), dCamera);
	//FBX
	//モデル名を指定してファイル読み込み                  
	model_Player = FbxLoader::GetInstance()->LoadModelFromFile("armar");
	model_Ground = FbxLoader::GetInstance()->LoadModelFromFile("ground");
	model_Boss = FbxLoader::GetInstance()->LoadModelFromFile("boss");
	model_AttackObject = FbxLoader::GetInstance()->LoadModelFromFile("Esphere");
	model_EnemyAttackObject = FbxLoader::GetInstance()->LoadModelFromFile("Esphere");
	//OBJ
	modelcube = ObjLoder::CreateFromOBJ("skydome");

	//デバイスをセット
	Object3d::SetDevice(dxCommon->GetDevice());
	//カメラをセット
	Object3d::SetCamera(dCamera);
	//グラフィックスパイプライン生成
	Object3d::CreateGraphicsPipeline();

	ObjectObj::SetCamera(dCamera);
	ObjectObj::CreateGraphicsPipeline();

	//3Dオブジェクト生成とモデルのセット
	//FBX
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
	objectEAttack->SetPosition({ 10,5,30});
	objectEAttack->SetScale({ 4,4,4 });
	object_Player = Player::Create(model_Player);
	object_Player->SetPosition({ 0,0,-30 });
	object_Player->SetRotation({ 0,0,0 });

	//OBJ
	objcube = ObjectObj::Create(modelcube);
	objcube->SetPosition({0,15,10});
	objcube->SetScale({10,10,10});

	state = "OFF";
	timer = 0;
	sceneChengeFlag = false;

	fog->ResetFade(1.0f, fog->GetFadeOutFlag());

	// カメラ注視点をセット
	dCamera->SetTarget({ 0,1,0 });
	dCamera->SetDistance(25.0f);
}

void GameScene::Update()
{
	//カメラ更新
	debugCamera->Update();
	//ゲームパッド更新
	gamepad->Update();
	//パーティクル生成
	particleMan->Update();
	Vector3 pos = object_Player->GetPosition();
	
	object_Ground->Update();
	objcube->Update();
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

	if (spriteWarning->isMin())
	{
		objectBoss->Update(object_Player.get(), objectEAttack.get(), audio);
	}

	if (object_Player->GetLifeFlag() && spriteWarning->isMin())
	{
		object_Player->Update(debugCamera, objectAttack.get(), objectBoss.get(), audio);
	}

	int bossHp = objectAttack->GetHP();
	int playerHp = object_Player->GetPlayerHP();
	sprite2->SetSize(bossHp, 30);
	sprite4->SetSize(playerHp, 300);

	if (playerHp == 0)
	{
		GotoEndScene(playerHp, 1);
	}
	if (bossHp == 0)
	{
		GotoEndScene(bossHp, 2);
	}
}

void GameScene::Draw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//3Dオブジェクトの描画
	if (object_Player->GetLifeFlag())
	{
		object_Player->Draw(cmdList);
	}
	object_Ground->Draw(cmdList);
	objectBoss->Draw(cmdList);
	const bool DrawFlag = object_Player->GetColFlag();//当たり判定描画フラグ
	const bool DrawBossCollisionFlag = objectBoss->GetColFlag();
	
	if (DrawBossCollisionFlag)
	{
		objectEAttack->Draw(cmdList);
	}
	
	//kookok
	//if (DrawFlag)
	//{
	//	objectAttack->Draw(cmdList);
	//}
}

void GameScene::FogDraw()
{
	//// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	ObjectObj::PreDraw(dxCommon->GetCommandList());
	objcube->Draw();

	particleMan->Draw(cmdList);
	ObjectObj::PostDraw();
}

void GameScene::UIDraw()
{
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//pPos[0] = object1->GetPosition().x;
	//pPos[1] = object1->GetPosition().y;
	//pPos[2] = object1->GetPosition().z;

	//Time[0] = object1->GetTime();
	//Timerate[0] = object1->GetTimeRate();
	//BTime[0] = objectBoss->GetTime();

	//BTimerate[0] = objectBoss->GetTimeRate();
	//HP[0] = objectAttack->GetHP();
	//Stamina[0] = object1->Getstamina();

	//timerflag = object1->GetTimerFlag();
	//PHp[0] = object1->GetPlayerHP();
	//Bpos[0] = objectBoss->GetPosition().x;
	//Bpos[1] = objectBoss->GetPosition().y;
	//Bpos[2] = objectBoss->GetPosition().z;

	//Bros[0] = objectBoss->GetRotation().x;
	//Bros[1] = objectBoss->GetRotation().y;
	//Bros[2] = objectBoss->GetRotation().z;

	//Angle[0] = objectBoss->GetAngle();

	//vectorPos[0] = objectBoss->Getresult().x;
	//vectorPos[1] = objectBoss->Getresult().y;
	//vectorPos[2] = objectBoss->Getresult().z;

	//rot[0] = object1->GetRotation().x;
	//rot[1] = object1->GetRotation().y;
	//rot[2] = object1->GetRotation().z;

	//BApos[0] = objectEAttack->GetPosition().x;
	//BApos[1] = objectEAttack->GetPosition().y;
	//BApos[2] = objectEAttack->GetPosition().z;

	//dotVec[0] = objectBoss->GetDotVector();

	//viewAngle[0] = objectBoss->GetradiusCos();

	//ImGui::Begin("player");
	//ImGui::SetWindowPos(ImVec2(0, 0));
	//ImGui::SetWindowSize(ImVec2(500, 200));
	//ImGui::InputFloat3("Pos", pPos);
	//ImGui::InputFloat3("TotalTime", Time);
	//ImGui::InputFloat3("TimeRate", Timerate);
	//ImGui::InputFloat3("PlayerHP", PHp);
	//ImGui::InputFloat3("Stamina",Stamina);
	//ImGui::InputFloat3("Rotation",rot);
	//ImGui::End();

	//ImGui::Begin("BOSS");
	//ImGui::SetWindowPos(ImVec2(600, 0));
	//ImGui::SetWindowSize(ImVec2(500, 200));
	//ImGui::InputFloat3("BOSSPos", Bpos);
	//ImGui::InputFloat3("BossDotVector", dotVec);
	//ImGui::InputFloat3("siakaku", viewAngle);
	//ImGui::InputFloat3("TotalTime", BTime);
	//ImGui::InputFloat3("TimeRate", BTimerate);
	//ImGui::InputInt("BOSSHP", HP);
	//ImGui::End();

	// コマンドリストの取得
	//ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//スプライトのパイプラインをセット
	Sprite::SetPipelineState(dxCommon->GetCommandList());
	// 前景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	sprite3->Draw(dxCommon->GetCommandList());
	sprite2->Draw(dxCommon->GetCommandList());
	sprite5->Draw(dxCommon->GetCommandList());
	sprite4->Draw(dxCommon->GetCommandList());
	spriteWarning->Draw(dxCommon->GetCommandList());

	// スプライト描画後処理
	Sprite::PostDraw();
}

void GameScene::BgDraw()
{
	// コマンドリストの取得
	ID3D12GraphicsCommandList* cmdList = dxCommon->GetCommandList();
	//スプライトのパイプラインをセット
	Sprite::SetPipelineState(dxCommon->GetCommandList());
	// 背景スプライト描画前処理
	Sprite::PreDraw(cmdList);
	// スプライト描画後処理
	Sprite::PostDraw();
	// 深度バッファクリア
	dxCommon->ClearRenderTarget();
}

void GameScene::CreateParticles()
{
	for (int i = 0; i < 10; i++) {
		// X,Y,Z全て[-5.0f,+5.0f]でランダムに分布
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

		// 追加
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
		//フェードアウト開始
		if (timer >= 200)
		{
			ChangeSceneChengeFlag();
		}

		if (timer >= endTime)
		{
			timer = 0;
			//エンドシーンへ
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
		
		//フェードアウト開始
		if (timer >= 200)
		{
			ChangeSceneChengeFlag();
		}

		if (timer >= endTime)
		{
			timer = 0;
			//エンドシーンへ
			SceneManager::GetInstance()->SetSceneState(Scene::End);
			SceneManager::GetInstance()->ChangeScene("END");
		}
	}
}