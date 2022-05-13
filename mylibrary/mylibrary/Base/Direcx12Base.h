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
private://エイリアス
	template<class T> using ComPtr = Microsoft::WRL::ComPtr<T>;
public://メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(windowsApi* winApp);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void BeginDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void EndDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
    /// imgui初期化
    /// </summary>
    /// <returns>成否</returns>
	bool InitImgui();

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns></returns>
	ID3D12Device* GetDevice() { return dev.Get();}

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns></returns>
	ID3D12GraphicsCommandList* GetCommandList() { return cmdList.Get(); }

	/// <summary>
	/// キューの取得
	/// </summary>
	/// <returns></returns>
	ID3D12CommandQueue* GetCommandQueue() { return cmdQueue.Get(); }

private://メンバ変数
	//ウィンドウズアプリ管理
	windowsApi* winApp;

	//3D関連
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

