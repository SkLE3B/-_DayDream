#pragma once
#include <Effekseer.h>
#include <EffekseerRendererDX12.h>
#include<memory>
#include "base/Direcx12Base.h"
#include "math/Vector3.h"
#include "Camera/Camera.h"
#include "Camera/DebugCamera.h"
#include "Input/Input.h"

class EffekseerManager
{
private://�G�C���A�X
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
	//Directx::���ȗ�
	using XMFLOAT2 = DirectX::XMFLOAT2;
	using XMFLOAT3 = DirectX::XMFLOAT3;
	using XMFLOAT4 = DirectX::XMFLOAT4;
	using XMMATRIX = DirectX::XMMATRIX;
	using XMVECTOR = DirectX::XMVECTOR;
public:
	static void Initialize(Direcx12Base* dxBase,Camera* camera);

	static void Update(Direcx12Base* dxBase, Camera* camera);

	static void Draw();

	static void Finalize();

	/// <summary>
	/// �G�t�F�N�g�Đ�
	/// </summary>
	/// <param name="arg_effectName"></param>
	/// <param name="arg_position"></param>
	/// <returns></returns>
	static Effekseer::Handle PlayEffect(char16_t* arg_effectName, const Vector3& arg_position);

	/// <summary>
	/// �G�t�F�N�g�擾
	/// </summary>
	/// <param name="arg_effectName"></param>
	/// <returns></returns>
	static Effekseer::EffectRef GetEffect(char16_t* arg_effectName);

	/// <summary>
	/// �G�t�F�N�g��~
	/// </summary>
	/// <param name="handle"></param>
	/// <returns></returns>
	static Effekseer::Handle StopEffect(const Effekseer::Handle handle);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="arg_handle"></param>
	/// <param name="arg_position"></param>
	static void SetPosition(const Effekseer::Handle& arg_handle, const Vector3& arg_position);

	/// <summary>
	/// 
	/// </summary>
	/// <param name="arg_handle"></param>
	/// <param name="arg_scale"></param>
	static void SetScale(const Effekseer::Handle& arg_handle, const Vector3& arg_scale);

	/// <summary>
    /// 
    /// </summary>
    /// <param name="arg_handle"></param>
    /// <param name="arg_scale"></param>
	static void SetRotation(const Effekseer::Handle& arg_handle, const Vector3& arg_rotation);

private://�ÓI�����o�ϐ�
//��ʃN���A�J���[
	static const float clearColor[4];
	// �J����
	static Camera* camera;
private:
	static Direcx12Base* dxBase;
	//DXGI_FORMAT format;
	static EffekseerRenderer::RendererRef renderer;
	static Effekseer::ManagerRef manager;
	static Effekseer::RefPtr<EffekseerRenderer::SingleFrameMemoryPool> memoryPool;
	static Effekseer::RefPtr <EffekseerRenderer::CommandList> cmdListEfk;
	static Effekseer::EffectRef effect;
	static Effekseer::Vector3D g_position;
	static int32_t time;
	static Effekseer::Handle handle;
	static std::unordered_map<std::string, Effekseer::EffectRef> map_effects;
	ComPtr<ID3D12Resource> texBuff;
	//RTV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapRTV;
	//DSV�p�f�X�N���v�^�q�[�v
	ComPtr<ID3D12DescriptorHeap> descHeapDSV;
};

