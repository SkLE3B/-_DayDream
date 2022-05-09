#include "EffekseerManager.h"
#include<DirectXMath.h>
#include "Camera/DebugCamera.h"

EffekseerRenderer::RendererRef  EffekseerManager::renderer;
Effekseer::ManagerRef EffekseerManager::manager;
Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> EffekseerManager::memoryPool;
Effekseer::RefPtr <EffekseerRenderer::CommandList> EffekseerManager::cmdListEfk;

Effekseer::EffectRef EffekseerManager::effect;
Effekseer::Vector3D  EffekseerManager::g_position;
Effekseer::Handle EffekseerManager::handle;
int32_t  EffekseerManager::time;
std::unordered_map<std::string, Effekseer::EffectRef> EffekseerManager::map_effects;
Direcx12Base* EffekseerManager::dxBase;

Camera* EffekseerManager::camera = nullptr;

void EffekseerManager::Initialize(Direcx12Base* dxBase, Camera* camera)
{
	// Effekseerのオブジェクトはスマートポインタで管理される。変数がなくなると自動的に削除される。
    // ただし、COMの終了前に削除されるとおかしくなるので、スコープを追加する。
	// エフェクトのレンダラーの作成
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderer = EffekseerRendererDX12::Create(dxBase->GetDevice(), dxBase->GetCommandQueue(), 3, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);
	//メモリプールの作成
	memoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());
	//コマンドリストの作成
	cmdListEfk = EffekseerRenderer::CreateCommandList(renderer->GetGraphicsDevice(), memoryPool);

	//エフェクトマネージャーの作成
	manager = Effekseer::Manager::Create(8000);
	
	//描画モジュールの設定
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	// テクスチャ、モデル、カーブ、マテリアルローダーの設定する。
    // ユーザーが独自で拡張できる。現在はファイルから読み込んでいる。
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());
	manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	// 投影行列を設定
	renderer->SetProjectionMatrix(
		Effekseer::Matrix44().PerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), (float)1280 / (float)720, 0.1f, 1000.0f));

	time = 0;
	handle = 0;

	//テクスチャリソース設定
	CD3DX12_RESOURCE_DESC textureDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_R8G8B8A8_UNORM,
		windowsApi::window_width,
		(UINT)windowsApi::window_height,
		1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	);

	HRESULT result;
}

void EffekseerManager::Update(Direcx12Base* dxBase, Camera* camera)
{
	// フレームの開始時に呼ぶ
	memoryPool->NewFrame();

	// コマンドリストを開始する。
	EffekseerRendererDX12::BeginCommandList(cmdListEfk, dxBase->GetCommandList());
	renderer->SetCommandList(cmdListEfk);

	//カメラ行列を更新
	renderer->SetCameraMatrix(
		Effekseer::Matrix44().LookAtLH(camera->GetEye().ConvertEffeckseerVector3(camera->GetEye()), camera->GetTarget().ConvertEffeckseerVector3(camera->GetTarget()), camera->GetUp().ConvertEffeckseerVector3(camera->GetUp())));

	// マネージャーの更新
	manager->Update();

	// 時間を更新する
	renderer->SetTime(time / 60.0f);

	time++;
}

void EffekseerManager::Draw()
{
	// エフェクトの描画開始処理を行う。
	renderer->BeginRendering();
	// Render effects
	// エフェクトの描画を行う。
	manager->Draw();
	// エフェクトの描画終了処理を行う。
	renderer->EndRendering();
}

void EffekseerManager::Finalize()
{
	// マネージャーの破棄
	manager.Reset();

	// レンダラーの破棄
	renderer.Reset();
}

Effekseer::Handle EffekseerManager::PlayEffect(char16_t* arg_effectName, const Vector3& arg_position)
{
	auto handle = manager->Play(GetEffect(arg_effectName), arg_position.x, arg_position.y, arg_position.z);

	return handle;
}

Effekseer::EffectRef EffekseerManager::GetEffect(char16_t* arg_effectName)
{
	effect = Effekseer::Effect::Create(manager, arg_effectName);

	return effect;
}

Effekseer::Handle EffekseerManager::StopEffect(const Effekseer::Handle handle)
{
	manager->StopEffect(handle);

	return handle;
}

void EffekseerManager::SetPosition(const Effekseer::Handle& arg_handle, const Vector3& arg_position)
{
	manager->SetLocation(arg_handle, Effekseer::Vector3D(arg_position.x, arg_position.y, arg_position.z));
}

void EffekseerManager::SetScale(const Effekseer::Handle& arg_handle, const Vector3& arg_scale)
{
	manager->SetScale(arg_handle, arg_scale.x, arg_scale.y, arg_scale.z);
}

void EffekseerManager::SetRotation(const Effekseer::Handle& arg_handle, const Vector3& arg_rotation)
{
	manager->SetRotation(arg_handle, arg_rotation.x, arg_rotation.y, arg_rotation.z);
}