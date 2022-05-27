#include "GameBase.h"

using namespace std;

void GameBase::Run()
{
	//������
	Initialize();

	while (true)
	{
		//�I���t���O�ɂ���ă��[�v�𔲂���
		if (endFlag){
			break;
		}

		//Escape�L�[�Ń��[�v�𔲂���
		if (input->IsPush(DIK_ESCAPE))
		{
			break;
		}

		//�X�V
		Update();

		//�`��
		Draw();
	
	}
	//��n��
	Finalize();
}

void GameBase::Initialize()
{
#pragma region Windows�A�v��������
	winApp = make_unique<windowsApi>();
	winApp->Initialize();
#pragma endregion

#pragma region Directx12������

	dxBase = make_unique<Direcx12Base>();
	dxBase->Initialize(winApp.get());

#pragma endregion

#pragma region �ėp�@�\������
	//�e�N�X�`���}�l�[�W���[������
	textureManager = make_unique<TextureManager>();
	textureManager->Initialize(dxBase.get());

	//�C���v�b�g������
	input = Input::GetInstance();
	input->Initialize(winApp->GetWndClass().hInstance, winApp->GetHwnd());

	//�I�[�f�B�I�}�l�[�W���[
	audioManger = make_unique<AudioManager>();
	audioManger->Initialize();

	//�X�v���C�g�̋��ʏ�����
	Sprite::StaticInitialize(dxBase.get(), textureManager.get());

	//�f�o�b�O�e�L�X�g�p�̃e�N�X�`���ԍ����w��
	const int debugTextTexNumber = 2;
	//�f�o�b�O�e�L�X�g�p�e�N�X�`���ǂݍ���
	textureManager->spriteLoadTexture(debugTextTexNumber, L"Resources/textures/debugfont.png");

	//�f�o�b�O�e�L�X�g������
	debugText = DebugText::GetInstance();
	debugText->Initialize(debugTextTexNumber);

	//���f���̏�����
	model = make_unique<GeometoryModel>();
	model->Initialize(dxBase.get(), textureManager.get(), 1);

	//�f�o�b�O�J�����̏�����
	Dcamera = make_unique<DebugCamera>();
	Dcamera->Initialize(winApp->window_width, winApp->window_height, input);
	Bcamera = make_unique<BackCamera>();
	Bcamera->Initialize(winApp->window_width, winApp->window_height);

	//�􉽊w�I�u�W�F�N�g�̋��ʏ�����
	GeometoryObject::StaticInitialize(dxBase.get(), model.get(),Dcamera.get());
	//FBX���[�_�[������
	FbxLoader::GetInstance()->Initialize(dxBase->GetDevice());
	//�Q�[���p�b�h�̏�����
	gamepad = make_unique<Gamepad>();
	gamepad->Initialize();

	//�|�X�g�G�t�F�N�g�p�e�N�X�`���ǂݍ���
	textureManager->spriteLoadTexture(4, L"Resources/textures/tex1.png");
	//�|�X�g�G�t�F�N�g������
	postEffect = new PostEffect();
	postEffect->Initialize(Dcamera.get());

	//�t�H�O
	testfog = make_unique<Fog>();
	testfog->Initialize(Dcamera.get());

	ObjectObj::StaticInitialize(dxBase->GetDevice());
	flamerate = make_unique<FlameRate>();

	EffekseerManager::Initialize(dxBase.get(),Dcamera.get());
#pragma endregion
}

void GameBase::Update()
{
	//WindowsAPI���t���[������
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