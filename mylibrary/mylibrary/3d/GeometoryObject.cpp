#include "GeometoryObject.h"
#include <cassert>
#include<d3dcompiler.h>

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

//静的メンバ変数の実体
GeometoryObject::Common* GeometoryObject::common = nullptr;

void GeometoryObject::Common::InitializeGraphicsPipeline(Direcx12Base* dxBase)
{
	HRESULT result = S_FALSE;

	//各シェーダファイルの読み込みとコンパイル
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/BasicVertexShader.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"VSmain", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/BasicPixelShader.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"PSmain", "ps_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&psBlob, &errorBlob);

	//シェーダーのエラー内容を表示
	if (FAILED(result)) {
		// errorBlob からエラー内容を string 型にコピー
		std::string errstr;
		errstr.resize(errorBlob->GetBufferSize());
		std::copy_n((char*)errorBlob->GetBufferPointer(),
			errorBlob->GetBufferSize(),
			errstr.begin());
		errstr += "\n";
		// エラー内容を出力ウィンドウに表示
		OutputDebugStringA(errstr.c_str());
		exit(1);
	}

	//頂点シェーダに渡すための頂点データを整える
	D3D12_INPUT_ELEMENT_DESC inputLayout[] =
	{//xyz座標
	  {
		 "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0,
		  D3D12_APPEND_ALIGNED_ELEMENT,
		  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0
	  },
		//法線ベクトル
		  {
			  "NORMAL",0,DXGI_FORMAT_R32G32B32_FLOAT,0,
			  D3D12_APPEND_ALIGNED_ELEMENT,
			  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		  },
		{//uv座標
			"TEXCOORD",0,DXGI_FORMAT_R32G32_FLOAT,0,
			D3D12_APPEND_ALIGNED_ELEMENT,
			D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,0
		},
	};

	//グラフィックスパイプラインの各ステージの設定をする構造体を用意する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC gpipeline{};

	//頂点シェーダ、ピクセルシェーダをパイプラインに設定
	gpipeline.VS = CD3DX12_SHADER_BYTECODE(vsBlob);
	gpipeline.PS = CD3DX12_SHADER_BYTECODE(psBlob);

	//サンプルマスクとラスタライザステートの設定
	gpipeline.SampleMask = D3D12_DEFAULT_SAMPLE_MASK; // 標準設定
	//標準的な設定(背面力リング、塗りつぶし、深度クリッピング有効)
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//標準的な設定(深度設定を行う,書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	//レンダーターゲットのブレンド設定(8個あるがいまは1つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//共通設定
	blenddesc.BlendEnable = true;

	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//頂点ステートの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	gpipeline.NumRenderTargets = 1; // 描画対象は 1 つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255 指定の RGBA
	gpipeline.SampleDesc.Count = 1; // 1 ピクセルにつき 1 回サンプリング

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeCBV, descRangeSRV;
	descRangeCBV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);//b0レジスタ
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0レジスタ

	////ルートパラメーター
	CD3DX12_ROOT_PARAMETER rootparams[2];
	rootparams[0].InitAsConstantBufferView(0);
	rootparams[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparams), rootparams, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);

	//ルートシグネチャの生成
	result = dxBase->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootsignature));
	assert(SUCCEEDED(result));

	//グラフィックスパイプラインステートの設定にセット
	gpipeline.pRootSignature = rootsignature.Get();
	assert(SUCCEEDED(result));
	//パイプラインにルートシグネチャをセット
	result = dxBase->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&pipelinestate));
	assert(SUCCEEDED(result));
}

void GeometoryObject::Common::InitializeDescriptorHeap(Direcx12Base* dxBase)
{
	HRESULT result = S_FALSE;

	////バッファを確保するので新たに専用のデスクリプタヒープを作成する
    ////設定構造体
	D3D12_DESCRIPTOR_HEAP_DESC descHeapDesc{};
	descHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;//シェーダーから見える
	descHeapDesc.NumDescriptors = maxObjectCount; //CBV
	//生成
	result = dxBase->GetDevice()->CreateDescriptorHeap(&descHeapDesc, IID_PPV_ARGS(&basicDescHeap));
	assert(SUCCEEDED(result));
}

void GeometoryObject::Common::InitializeCamera()
{
	//呼び出し
	
}

void GeometoryObject::StaticInitialize(Direcx12Base* dxBase, GeometoryModel* model,Camera* camera)
{
	common = new Common();

	common->dxBase = dxBase;
	//グラフィックスパイプラインの初期化
	common->InitializeGraphicsPipeline(dxBase);
	//デスクリプタヒープの初期化
	common->InitializeDescriptorHeap(dxBase);
	//カメラ初期化
	common->InitializeCamera();

	common->model = model;
	common->camera = camera;
}

void GeometoryObject::StaticFinalize()
{
	delete common;
	common = nullptr;
}

void GeometoryObject::Initialize()
{
	HRESULT result = S_FALSE;
	//定数バッファの生成
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD),//アップロード可能
		D3D12_HEAP_FLAG_NONE,
		&CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffurData) + 0xff)&~0xff),
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	assert(SUCCEEDED(result));

	//定数バッファビューの生成
	assert(common->descHeapIndex <= maxObjectCount - 1);

	//デスクリプタ1つ分のバッファサイズ
	UINT descHandleIncrementSize =common->dxBase->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//デスクリプタヒープ内での定数バッファビューのアドレスを計算
	cpuDescHandleCBV = CD3DX12_CPU_DESCRIPTOR_HANDLE(common->basicDescHeap->GetCPUDescriptorHandleForHeapStart(), common->descHeapIndex, descHandleIncrementSize);
	gpuDescHandleCBV = CD3DX12_GPU_DESCRIPTOR_HANDLE(common->basicDescHeap->GetGPUDescriptorHandleForHeapStart(), common->descHeapIndex, descHandleIncrementSize);

	//定数バッファビューの作成
	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
	cbvDesc.BufferLocation = constBuff->GetGPUVirtualAddress();
	cbvDesc.SizeInBytes = (UINT)constBuff->GetDesc().Width;
	common->dxBase->GetDevice()->CreateConstantBufferView(
		&cbvDesc, cpuDescHandleCBV);
	//デスクリプタヒープの使用番号を１進める
	common->descHeapIndex++;
}

void GeometoryObject::Update()
{
	HRESULT result;
	XMMATRIX matScale, matRot, matTrans;

	//スケール、回転、平行移動行列の計算
	matScale = XMMatrixScaling(scale.x, scale.y, scale.z);
	matRot = XMMatrixIdentity();
	matRot *= XMMatrixRotationZ(XMConvertToRadians(rotation.z));
	matRot *= XMMatrixRotationX(XMConvertToRadians(rotation.x));
	matRot *= XMMatrixRotationY(XMConvertToRadians(rotation.y));
	matTrans = XMMatrixTranslation(position.x, position.y, position.z);
	
	//ワールド行列の合成
	matWorld = XMMatrixIdentity();//変形をリセット
	matWorld *= matScale;//ワールド行列にスケーリングを反映
	matWorld *= matRot;//ワールド行列に回転を反映
	matWorld *= matTrans;//ワールド行列に平行移動を反映

	//親オブジェクトがあれば
	if (parent != nullptr)
	{
		 //親オブジェクトのワールド行列を掛ける
		matWorld *= parent->matWorld;
	}

	//カメラの行列取得
	const XMMATRIX& matView = common->camera->GetMatView();
	const XMMATRIX& matProjection = common->camera->GetMatProjection();

	//定数バッファへデータ転送
	ConstBuffurData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = XMFLOAT4(1, 1, 1, 1);//RGBA
	constMap->mat = matWorld * matView * matProjection;//行列の合成
	constBuff->Unmap(0, nullptr);
}

void GeometoryObject::Draw(ID3D12GraphicsCommandList* cmdList)
{
#pragma region 共通描画コマンド
	//パイプラインステートの設定コマンド
	cmdList->SetPipelineState(common->pipelinestate.Get());
	//ルートシグネチャの設定コマンド
	cmdList->SetGraphicsRootSignature(common->rootsignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
#pragma endregion

#pragma region 個別描画コマンド
	//定数バッファセット
    cmdList->SetGraphicsRootConstantBufferView(0, constBuff->GetGPUVirtualAddress());//ヒープの先頭がバッファ	

    common->model->Draw(cmdList);//モデル描画
#pragma endregion
}

void GeometoryObject::ResetDescriptorHeap()
{
	common->descHeapIndex = 0;
}
