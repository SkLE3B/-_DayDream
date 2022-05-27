#include "GameBase.h"

using namespace std;

void GameBase::Run()
{
	//初期化
	Initialize();

	while (true)
	{
		//終了フラグによってループを抜ける
		if (endFlag){
			break;
		}

		//Escapeキーでループを抜ける
		if (input->IsPush(DIK_ESCAPE))
		{
			break;
		}

		//更新
		Update();

		//描画
		Draw();
	
	}
	//後始末
	Finalize();
}

void GameBase::Initialize()
{
#pragma region Windowsアプリ初期化
	winApp = make_unique<windowsApi>();
	winApp->Initialize();
#pragma endregion

#pragma region Directx12初期化

	dxBase = make_unique<Direcx12Base>();
	dxBase->Initialize(winApp.get());

#pragma endregion

#pragma region 汎用機能初期化
	//テクスチャマネージャー初期化
	textureManager = make_unique<TextureManager>();
	textureManager->Initialize(dxBase.get());

	//インプット初期化
	input = Input::GetInstance();
	input->Initialize(winApp->GetWndClass().hInstance, winApp->GetHwnd());

	//オーディオマネージャー
	audioManger = make_unique<AudioManager>();
	audioManger->Initialize();

	//スプライトの共通初期化
	Sprite::StaticInitialize(dxBase.get(), textureManager.get());

	//デバッグテキスト用のテクスチャ番号を指定
	const int debugTextTexNumber = 2;
	//デバッグテキスト用テクスチャ読み込み
	textureManager->spriteLoadTexture(debugTextTexNumber, L"Resources/textures/debugfont.png");

	//デバッグテキスト初期化
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	//モデルの初期化
	model = make_unique<GeometoryModel>();
	model->Initialize(dxBase.get(), textureManager.get(), 1);

	//デバッグカメラの初期化
	Dcamera = make_unique<DebugCamera>();
	Dcamera->Initialize(winApp->window_width, winApp->window_height, input);
	Bcamera = make_unique<BackCamera>();
	Bcamera->Initialize(winApp->window_width, winApp->window_height);

	//幾何学オブジェクトの共通初期化
	GeometoryObject::StaticInitialize(dxBase.get(), model.get(),Dcamera.get());
	//FBXローダー初期化
	FbxLoader::GetInstance()->Initialize(dxBase->GetDevice());
	//ゲームパッドの初期化
	gamepad = make_unique<Gamepad>();
	gamepad->Initialize();

	//ポストエフェクト用テクスチャ読み込み
	textureManager->spriteLoadTexture(4, L"Resources/textures/tex1.png");
	//ポストエフェクト初期化
	postEffect = new PostEffect();
	postEffect->Initialize(Dcamera.get());

	//フォグ
	testfog = make_unique<Fog>();
	testfog->Initialize(Dcamera.get());

	ObjectObj::StaticInitialize(dxBase->GetDevice());
	flamerate = make_unique<FlameRate>();

	EffekseerManager::Initialize(dxBase.get(),Dcamera.get());
#pragma endregion
}

void GameBase::Update()
{
	//WindowsAPI毎フレーム処理
	if (winApp->Update())
	{
		endFlag = true;
	}

	input->Update();
	flamerate->Update();
}

void GameBase::Finalize()
{
	GeometoryObject::StaticFinalize();
	Sprite::StaticFinalize();
	FbxLoader::GetInstance()->Finalize();
	PostEffect::StaticFinalize();
	delete sceneFactory;
}