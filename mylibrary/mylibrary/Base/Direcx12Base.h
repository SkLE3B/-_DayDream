#pragma once

#include<dxgi1_6.h>
#include<vector>
#include<d3d12.h>
#include<d3dx12.h>
#include <imgui.h>
#include<wrl.h>
#include"windowsApi.h"

class Direcx12Base
{
private://�G�C���A�X
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://�����o�֐�
	/// <summary>
	/// ������
	/// </summary>
	void Initialize(windowsApi* winApp);

	/// <summary>
	/// �`��O����
	/// </summary>
	void BeginDraw();

	/// <summary>
	/// �`��㏈��
	/// </summary>
	void EndDraw();

	/// <summary>
	/// �����_�[�^�[�Q�b�g�̃N���A
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
    /// imgui������
    /// </summary>
    /// <returns>����</returns>
	bool InitImgui();

	/// <summary>
	/// �f�o�C�X�̎擾
	/// </summary>
	/// <returns></returns>
	ID3D12Device* GetDevice() { return dev.Get();}

	/// <summary>
	/// �`��R�}���h���X�g�̎擾
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList() { return cmdList.Get(); }

	/// <summary>
	/// �L���[�̎擾
	/// </summary>
	/// <returns></returns>
	ID3D12CommandQueue* GetCommandQueue() { return cmdQueue.Get(); }

private://�����o�ϐ�
	//�E�B���h�E�Y�A�v���Ǘ�
	windowsApi* winApp;

	//3D�֘A
	ComPtr<ID3D12Device> dev;
	ComPtr<IDXGIFactory6> dxgiFactory;
	ComPtr<IDXGISwapChain4> swapchain;
	ComPtr<ID3D12CommandAllocator>cmdAllocator;
	ComPtr<ID3D12GraphicsCommandList>cmdList;
	std::vector<ComPtr<ID3D12Resource>> backBuffers;//(2);
	ComPtr<ID3D12Resource> depthBuffer;
	ComPtr<ID3D12CommandQueue> cmdQueue;
	ComPtr<ID3D12DescriptorHeap>rtvHeps;
	ComPtr<IDXGIAdapter>tmpAdapter;
	ComPtr<ID3D12Fence> fence;
	UINT64 fenceVal = 0;

	ComPtr<ID3D12DescriptorHeap> imguiHeap;
	D3D12_RESOURCE_BARRIER barrierDesc;
	ComPtr<ID3D12DescriptorHeap> dsvHeap;
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc;
	UINT bbindex;
};

