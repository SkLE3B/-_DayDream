#include "Direcx12Base.h"
#include <imgui_impl_win32.h>
#include <imgui_impl_dx12.h>
#include <cassert>

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib,"dxguid.lib")

void Direcx12Base::Initialize(windowsApi* winApp)
{
	this->winApp = winApp;

	HRESULT result = S_FALSE;
#ifdef _DEBUG
	//デバッグレイアーをオンに
	ComPtr<ID3D12Debug> debugController;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
	{
		debugController->EnableDebugLayer();
	}
#endif

	//対応レベルの配列
	D3D_FEATURE_LEVEL levels[] =
	{
		D3D_FEATURE_LEVEL_12_1,
		D3D_FEATURE_LEVEL_12_0,
		D3D_FEATURE_LEVEL_11_1,
		D3D_FEATURE_LEVEL_11_0,
	};

	//DXGI ファクトリーの生成
	result = CreateDXGIFactory1(IID_PPV_ARGS(&dxgiFactory));
	if (FAILED(result))
	{
		assert(0);
	}

	//アダプターの列挙用
	std::vector<ComPtr<IDXGIAdapter>> adapters;
	//ここに特定の名前を持つアダプターオブジェクトが入る
	for (int i = 0;
		dxgiFactory->EnumAdapters(i, &tmpAdapter) != DXGI_ERROR_NOT_FOUND;
		i++)
	{
		adapters.push_back(tmpAdapter);//動的配列に追加する
	}

	for (int i = 0; i < adapters.size(); i++)
	{
		DXGI_ADAPTER_DESC adesc{};
		adapters[i]->GetDesc(&adesc);//アダプターの情報を取得
		std::wstring strDesc = adesc.Description; //アダプター名
		//　Microsoft.Basic.Rendr Driverを回避
		if (strDesc.find(L"Microsoft") == std::wstring::npos)
		{
			tmpAdapter = adapters[i]; //採用
			break;
		}
	}

	D3D_FEATURE_LEVEL featureLevel;

	result = S_FALSE;
	for (int i = 0; i < _countof(levels); i++)
	{
		//採用したアダプターでデバイスを生成
		result = D3D12CreateDevice(tmpAdapter.Get(), levels[i], IID_PPV_ARGS(&dev));
		if (result == S_OK)
		{
			//デバイスを生成できた時点でループを抜ける
			featureLevel = levels[i];
			break;
		}
	}
	if (FAILED(result))
	{
		assert(0);
	}

	//コマンドアロケーターとコマンドリストの生成
	result = dev->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(&cmdAllocator));
	if (FAILED(result))
	{
		assert(0);
	}

	//コマンドリストを生成
	result = dev->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		cmdAllocator.Get(), nullptr, IID_PPV_ARGS(&cmdList));
	if (FAILED(result))
	{
		assert(0);
	}

	//標準設定でコマンドキューを生成
	D3D12_COMMAND_QUEUE_DESC cmdQueueDesc{};
	dev->CreateCommandQueue(&cmdQueueDesc, IID_PPV_ARGS(&cmdQueue));
	if (FAILED(result))
	{
		assert(0);
	}

		//// 各種設定をしてスワップチェーンを生成
	DXGI_SWAP_CHAIN_DESC1 swapchainDesc{};
	swapchainDesc.Width = windowsApi::window_width;
	swapchainDesc.Height = windowsApi::window_height;
	swapchainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // 色情報の書式
	swapchainDesc.SampleDesc.Count = 1; // マルチサンプルしない
	swapchainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER; // バックバッファ用
	swapchainDesc.BufferCount = 2; // バッファ数を2つに設定
	swapchainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // フリップ後は破棄
	swapchainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	//IDXGISwapChain1のCOmprtを用意
	ComPtr<IDXGISwapChain1> swapchain1;
	//スワップチェーンの生成
	dxgiFactory->CreateSwapChainForHwnd(
		cmdQueue.Get(),
		winApp->GetHwnd(),
		&swapchainDesc,
		nullptr,
		nullptr,
		&swapchain1);
	//生成したIDXGISwapChain1のオブジェクトをIDXGISwapChain4に変換する
	swapchain1.As(&swapchain);
	if (FAILED(result))
	{
		assert(0);
	}

	//各種設定をしてディスクリプターヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc{};
	heapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; //レンダーターゲットビュー
	heapDesc.NumDescriptors = 2; //裏表の2つ
	dev->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&rtvHeps));

	backBuffers.resize(2);
	for (int i = 0; i < 2; i++)
	{
		//スワップチェーンからバッファを取得
		result = swapchain->GetBuffer(i, IID_PPV_ARGS(&backBuffers[i]));
		//ディスクリプタヒープのハンドルを取得
		D3D12_CPU_DESCRIPTOR_HANDLE handle =
			rtvHeps->GetCPUDescriptorHandleForHeapStart();
		//裏か表でアドレスがずれる
		handle.ptr += i * dev->GetDescriptorHandleIncrementSize(heapDesc.Type);
		//レンダーターゲットビューの生成
		dev->CreateRenderTargetView(
			backBuffers[i].Get(),
			nullptr,
			handle);
	}

	 //リソース設定
	    CD3DX12_RESOURCE_DESC  depthResDesc = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		windowsApi::window_width,
		windowsApi::window_height,
		1, 0,
		1, 0,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
		
	CD3DX12_HEAP_PROPERTIES heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		CD3DX12_CLEAR_VALUE Clear_VALUE = CD3DX12_CLEAR_VALUE(DXGI_FORMAT_D32_FLOAT, 1.0f, 0);

	//リソースの生成
	result = dev->CreateCommittedResource(
		&heapProp,
		D3D12_HEAP_FLAG_NONE,
		&depthResDesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,//深度値書き込みに使用
		&Clear_VALUE,
		IID_PPV_ARGS(&depthBuffer));
	if (FAILED(result))
	{
		assert(0);
	}

	//コマンド発行のために深度ビューを生成する
	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapHesc{};
	dsvHeapHesc.NumDescriptors = 1;//深度ビューは1つ
	dsvHeapHesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;//デプスステンシルビュー
	result = dev->CreateDescriptorHeap(&dsvHeapHesc, IID_PPV_ARGS(&dsvHeap));
	if (FAILED(result))
	{
		assert(0);
	}

	//深度ビュー作成
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dev->CreateDepthStencilView(
		depthBuffer.Get(),
		&dsvDesc,
		dsvHeap->GetCPUDescriptorHandleForHeapStart()
	);

	//フェンスの生成
	result = dev->CreateFence(fenceVal, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));

	// デスクリプタヒープを生成
	D3D12_DESCRIPTOR_HEAP_DESC heapDesc1{};
	heapDesc1.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	heapDesc1.NumDescriptors = 1;
	heapDesc1.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	result = dev->CreateDescriptorHeap(&heapDesc1, IID_PPV_ARGS(&imguiHeap));
	if (FAILED(result)) {
		assert(0);
	}

	// スワップチェーンの情報を取得
	DXGI_SWAP_CHAIN_DESC swcDesc = {};
	result = swapchain->GetDesc(&swcDesc);
	if (FAILED(result)) {
		assert(0);
	}

	if (ImGui::CreateContext() == nullptr) {
		assert(0);
	}
	if (!ImGui_ImplWin32_Init(winApp->GetHwnd())) {
		assert(0);
	}
	if (!ImGui_ImplDX12_Init(
		GetDevice(),
		swcDesc.BufferCount,
		swcDesc.BufferDesc.Format,
		imguiHeap.Get(),
		imguiHeap->GetCPUDescriptorHandleForHeapStart(),
		imguiHeap->GetGPUDescriptorHandleForHeapStart()))
	{
		assert(0);
	}
}

void Direcx12Base::BeginDraw()
{
	//バックバッファの番号を取得(2つなので0番か1番)
	bbindex = swapchain->GetCurrentBackBufferIndex();

	CD3DX12_RESOURCE_BARRIER RESOURCE_BARRIER = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbindex].Get(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

	//表示状態から描画状態に変更
	cmdList->ResourceBarrier(1, &RESOURCE_BARRIER);

	//レンダーターゲットビュー用ディスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE rtvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvHeps->GetCPUDescriptorHandleForHeapStart(),
		bbindex, dev->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV));

	//深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	D3D12_CPU_DESCRIPTOR_HANDLE dsvH = dsvHeap->GetCPUDescriptorHandleForHeapStart();
	cmdList->OMSetRenderTargets(1, &rtvH, false, &dsvH);

	//全画面クリア         R    G     B    A
	float clearColor[] = { 1.0f,1.0f,1.0f,1.0f };//緑っぽい色
	
	cmdList->ClearRenderTargetView(rtvH, clearColor, 0, nullptr);
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	D3D12_VIEWPORT viewport{};
	viewport.Width = windowsApi::window_width;//横幅
	viewport.Height = windowsApi::window_height;//縦幅
	viewport.TopLeftX = 0;//左上X
	viewport.TopLeftY = 0;//左上Y
	viewport.MinDepth = 0.0f;//最小深度(0で良い)
	viewport.MaxDepth = 1.0f;//最小深度(1で良い)
	cmdList->RSSetViewports(1, &viewport);

	//シザー短形コマンド
	D3D12_RECT scissorrect{};
	scissorrect.left = 0; // 切り抜き座標左
	scissorrect.right = windowsApi::window_width;
	scissorrect.top = 0; // 切り抜き座標上
	scissorrect.bottom = windowsApi::window_height;
	cmdList->RSSetScissorRects(1, &scissorrect);

	// imgui開始
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void Direcx12Base::EndDraw()
{
	// imgui描画
	ImGui::Render();
	ID3D12DescriptorHeap* ppHeaps[] = { imguiHeap.Get() };
	cmdList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), cmdList.Get());

	//4.リソースバリアを戻す
	barrierDesc.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;//描画
	barrierDesc.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;//表示に

	CD3DX12_RESOURCE_BARRIER RESOURCE_BARRIER = CD3DX12_RESOURCE_BARRIER::Transition(backBuffers[bbindex].Get(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);

	//描画状態から表示状態に変更
	cmdList->ResourceBarrier(1, &RESOURCE_BARRIER);

	//命令のクローズ
	cmdList->Close();
	//コマンドリストの実行
	ID3D12CommandList* cmdLists[] = { cmdList.Get() };//コマンドリストのリストの配列
	cmdQueue->ExecuteCommandLists(1, cmdLists);
	//コマンドリストの実行完了を待つ
	cmdQueue->Signal(fence.Get(), ++fenceVal);
	if (fence->GetCompletedValue() != fenceVal)
	{
		HANDLE event = CreateEvent(nullptr, false, false, nullptr);
		fence->SetEventOnCompletion(fenceVal, event);
		WaitForSingleObject(event, INFINITE);
		CloseHandle(event);
	}

	cmdAllocator->Reset();//キューをクリア
	cmdList->Reset(cmdAllocator.Get(), nullptr);//再びコマンドリストをためる準備

	//バッファをフリップ(裏表の入れ替え)
	swapchain->Present(1, 0);
}

void Direcx12Base::ClearRenderTarget()
{
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH = CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvHeap->GetCPUDescriptorHandleForHeapStart());
	// 深度バッファのクリア
	cmdList->ClearDepthStencilView(dsvH, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

bool Direcx12Base::InitImgui()
{
	return false;
}
