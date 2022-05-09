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
	// Effekseer�̃I�u�W�F�N�g�̓X�}�[�g�|�C���^�ŊǗ������B�ϐ����Ȃ��Ȃ�Ǝ����I�ɍ폜�����B
    // �������ACOM�̏I���O�ɍ폜�����Ƃ��������Ȃ�̂ŁA�X�R�[�v��ǉ�����B
	// �G�t�F�N�g�̃����_���[�̍쐬
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	renderer = EffekseerRendererDX12::Create(dxBase->GetDevice(), dxBase->GetCommandQueue(), 3, &format, 1, DXGI_FORMAT_UNKNOWN, false, 8000);
	//�������v�[���̍쐬
	memoryPool = EffekseerRenderer::CreateSingleFrameMemoryPool(renderer->GetGraphicsDevice());
	//�R�}���h���X�g�̍쐬
	cmdListEfk = EffekseerRenderer::CreateCommandList(renderer->GetGraphicsDevice(), memoryPool);

	//�G�t�F�N�g�}�l�[�W���[�̍쐬
	manager = Effekseer::Manager::Create(8000);
	
	//�`�惂�W���[���̐ݒ�
	manager->SetSpriteRenderer(renderer->CreateSpriteRenderer());
	manager->SetRibbonRenderer(renderer->CreateRibbonRenderer());
	manager->SetRingRenderer(renderer->CreateRingRenderer());
	manager->SetTrackRenderer(renderer->CreateTrackRenderer());
	manager->SetModelRenderer(renderer->CreateModelRenderer());

	// �e�N�X�`���A���f���A�J�[�u�A�}�e���A�����[�_�[�̐ݒ肷��B
    // ���[�U�[���Ǝ��Ŋg���ł���B���݂̓t�@�C������ǂݍ���ł���B
	manager->SetTextureLoader(renderer->CreateTextureLoader());
	manager->SetModelLoader(renderer->CreateModelLoader());
	manager->SetMaterialLoader(renderer->CreateMaterialLoader());
	manager->SetCurveLoader(Effekseer::MakeRefPtr<Effekseer::CurveLoader>());

	// ���e�s���ݒ�
	renderer->SetProjectionMatrix(
		Effekseer::Matrix44().PerspectiveFovLH(DirectX::XMConvertToRadians(60.0f), (float)1280 / (float)720, 0.1f, 1000.0f));

	time = 0;
	handle = 0;

	//�e�N�X�`�����\�[�X�ݒ�
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
	// �t���[���̊J�n���ɌĂ�
	memoryPool->NewFrame();

	// �R�}���h���X�g���J�n����B
	EffekseerRendererDX12::BeginCommandList(cmdListEfk, dxBase->GetCommandList());
	renderer->SetCommandList(cmdListEfk);

	//�J�����s����X�V
	renderer->SetCameraMatrix(
		Effekseer::Matrix44().LookAtLH(camera->GetEye().ConvertEffeckseerVector3(camera->GetEye()), camera->GetTarget().ConvertEffeckseerVector3(camera->GetTarget()), camera->GetUp().ConvertEffeckseerVector3(camera->GetUp())));

	// �}�l�[�W���[�̍X�V
	manager->Update();

	// ���Ԃ��X�V����
	renderer->SetTime(time / 60.0f);

	time++;
}

void EffekseerManager::Draw()
{
	// �G�t�F�N�g�̕`��J�n�������s���B
	renderer->BeginRendering();
	// Render effects
	// �G�t�F�N�g�̕`����s���B
	manager->Draw();
	// �G�t�F�N�g�̕`��I���������s���B
	renderer->EndRendering();
}

void EffekseerManager::Finalize()
{
	// �}�l�[�W���[�̔j��
	manager.Reset();

	// �����_���[�̔j��
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