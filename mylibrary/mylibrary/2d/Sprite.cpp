#include "Sprite.h"
#include <d3dcompiler.h>
#include "../Base/Direcx12Base.h"

#pragma comment(lib,"d3dcompiler.lib")

using namespace DirectX;

Sprite::Common* Sprite::common = nullptr;
ID3D12GraphicsCommandList* Sprite::cmdList = nullptr;

void Sprite::StaticInitialize(Direcx12Base* dxBase, TextureManager* textureManager)
{
	common = new Common();
	common->textureManager = textureManager;

	common->dxBase = dxBase;

	//グラフィックスパイプライン生成
	common->InitializeGraphicsPipeline();

	//射影行列
	common->MatProjection = DirectX::XMMatrixOrthographicOffCenterLH(
		0.0f, windowsApi::window_width,
		windowsApi::window_height, 0.0f,
		0.0f, 1.0f);
}

void Sprite::StaticFinalize()
{
	//解放
	if (common != nullptr)
	{
		delete common;
		common = nullptr;
	}
}

void Sprite::SetPipelineState(ID3D12GraphicsCommandList* cmdList)
{
	//パイプラインステートの設定
	cmdList->SetPipelineState(common->PipelineState.Get());
	//ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(common->RootSignature.Get());
	//プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::Initialize(UINT texnumber)
{
	HRESULT result = S_FALSE;

	this->texnumber = texnumber;
	time = limit;

	if (common->textureManager->GetSpriteTexbuff(texnumber))
	{
		//テクスチャ情報取得
		D3D12_RESOURCE_DESC resDesc =
			common->textureManager->GetSpriteTexbuff(texnumber)->GetDesc();
		float width = (float)resDesc.Width;//横幅の横幅
		float height = (float)resDesc.Height;//画像の縦幅

		tex_width = (float)resDesc.Width;
		tex_height = (float)resDesc.Height;
	}

	CD3DX12_HEAP_PROPERTIES HEAP_PROPERTIES = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC RESOURCE_DESC = CD3DX12_RESOURCE_DESC::Buffer(sizeof(VertexPosUv) * 4);

	//頂点バッファ生成
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&HEAP_PROPERTIES,
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC,
		D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertBuff));
	
	if (FAILED(result))
	{
		assert(0);
	}

	//頂点バッファへのデータ転送
	TransferVertices();

	//頂点バッファビューの作成
	vbView.BufferLocation = vertBuff->GetGPUVirtualAddress();
	vbView.SizeInBytes = sizeof(VertexPosUv) * 4;
	vbView.StrideInBytes = sizeof(VertexPosUv);

	CD3DX12_HEAP_PROPERTIES HEAP_PROPERTIES1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	CD3DX12_RESOURCE_DESC RESOURCE_DESC1 = CD3DX12_RESOURCE_DESC::Buffer((sizeof(ConstBuffurData) + 0xff) & ~0xff);

	//定数バッファの生成
	result = common->dxBase->GetDevice()->CreateCommittedResource(
		&HEAP_PROPERTIES1,
		D3D12_HEAP_FLAG_NONE,
		&RESOURCE_DESC1,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		nullptr,
		IID_PPV_ARGS(&constBuff));
	if (FAILED(result))
	{
		assert(0);
	}

	//定数バッファにデータ転送
	ConstBuffurData* constMap = nullptr;
	result = constBuff->Map(0, nullptr, (void**)&constMap);
	if (SUCCEEDED(result))
	{
		constMap->color = Color;//色指定(RGBA)
		constMap->mat = common->MatProjection;//平行投影行列の合成
		constBuff->Unmap(0, nullptr);
	}
}

void Sprite::TransferVertices()
{
	HRESULT result = S_FALSE;

	//頂点データ
	VertexPosUv  vertices[vertNum];

	//左下、左上、右下、右上
	enum { LB, LT, RB, RT };

	float left = (0.0f - anchorpoint.x) * width;
	float right = (1.0f - anchorpoint.x) * width;
	float top = (0.0f - anchorpoint.y) * height;
	float bottom = (1.0f - anchorpoint.y) * height;

	if (isFlipX)
	{//左右入れ替え
		left = -left;
		right = -right;
	}

	if (isFlipY)
	{//左右入れ替え
		top = -top;
		bottom = -bottom;
	}

	vertices[LB].pos = { left,  bottom, 0.0f }; //左下
	vertices[LT].pos = { left,  top,    0.0f }; //左上
	vertices[RB].pos = { right, bottom, 0.0f };//右下
	vertices[RT].pos = { right, top,    0.0f };//右上
	
	//テクスチャ情報取得
	if (common->textureManager->GetSpriteTexbuff(texnumber))
	{
	   D3D12_RESOURCE_DESC resDesc =
		   common->textureManager->GetSpriteTexbuff(texnumber)->GetDesc();
	   
	   float tex_left = tex_x / resDesc.Width;
	   float tex_right = (tex_x + tex_width) / resDesc.Width;
	   float tex_top = tex_y / resDesc.Height;
	   float tex_bottom = (tex_y + tex_height) / resDesc.Height;
	   
	   vertices[LB].uv = { tex_left,tex_bottom };
	   vertices[LT].uv = { tex_left,tex_top };
	   vertices[RB].uv = { tex_right,tex_bottom };
	   vertices[RT].uv = { tex_right,tex_top };
	}

	//頂点バッファへのデータ転送
	VertexPosUv* vertMap = nullptr;
	result = vertBuff->Map(0, nullptr, (void**)&vertMap);
	memcpy(vertMap, vertices, sizeof(vertices));
	vertBuff->Unmap(0, nullptr);
}

void Sprite::Draw(ID3D12GraphicsCommandList* cmdList)
{
	//ワールド行列の更新
	spriteMatWorld = XMMatrixIdentity();
	spriteMatWorld *= XMMatrixRotationZ(XMConvertToRadians(rotation));
	spriteMatWorld *= XMMatrixTranslation(position.m128_f32[0],position.m128_f32[1],0.0f);

	//行列の転送
	ConstBuffurData* constMap = nullptr;
	HRESULT result = constBuff->Map(0, nullptr, (void**)&constMap);
	constMap->color = Color;
	constMap->mat = spriteMatWorld * common->MatProjection;//行列の合成
	constMap->Time = time;

	//フェードアウト
	FadeOut();

	constBuff->Unmap(0, nullptr);

	//頂点バッファをセット
	cmdList->IASetVertexBuffers(0, 1, &vbView);
	//定数バッファをセット
	cmdList->SetGraphicsRootConstantBufferView(0,constBuff->GetGPUVirtualAddress());
	//デスクリプタヒープの配列
	common->textureManager->SetDescriptorHeaps(cmdList);
	//シェーダーリソースビューをセット
	common->textureManager->SetShaderResourceView(cmdList, 1,texnumber);
	//描画コマンド
	cmdList->DrawInstanced(4, 1, 0, 0);
}

void Sprite::PreDraw(ID3D12GraphicsCommandList* cmdList)
{
	// PreDrawとPostDrawがペアで呼ばれていなければエラー
	assert(Sprite::cmdList == nullptr);

	// コマンドリストをセット
	Sprite::cmdList = cmdList;
	// パイプラインステートの設定
	cmdList->SetPipelineState(common->PipelineState.Get());
	// ルートシグネチャの設定
	cmdList->SetGraphicsRootSignature(common->RootSignature.Get());
	// プリミティブ形状を設定
	cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
}

void Sprite::PostDraw()
{
	// コマンドリストを解除
	Sprite::cmdList = nullptr;
}

void Sprite::SetSize(float width, float height)
{
	this->width = width;
	this->height = height;

	//頂点バッファに反映
	TransferVertices();
}

void Sprite::SetRotation(float rotation)
{
	this->rotation = rotation;

	//頂点バッファに反映
	TransferVertices();
}

void Sprite::SetAnchorPoint(XMFLOAT2 anchorpoint)
{
	this->anchorpoint = anchorpoint;

	//頂点バッファに反映
	TransferVertices();
}

void Sprite::SetPosition(float x, float y)
{
	this->position = { x,y,0,1 };

	//頂点バッファに反映
	TransferVertices();
}

void Sprite::SetIsFlipX(bool isFlipX)
{
	this->isFlipX = isFlipX;

	//頂点バッファに反映
	TransferVertices();
}

void Sprite::SetIsFlipY(bool isFlipY)
{
	this->isFlipY = isFlipY;

	//頂点バッファに反映
	TransferVertices();
}

void Sprite::SetTextureRange(float tex_x, float tex_y, float tex_width, float tex_height)
{
	this->tex_x = tex_x;
	this->tex_y = tex_y;
	this->tex_width = tex_width;
	this->tex_height = tex_height;
	
	width = tex_width;
	height = tex_height;
	
	//頂点バッファに反映
	TransferVertices();
}

void Sprite::SetTextureRect(float tex_x, float tex_y, float tex_width, float tex_height)
{
	this->tex_x = tex_x;
	this->tex_y = tex_y;
	this->tex_width = tex_width;
	this->tex_height = tex_height;

	this->width = tex_width;
	this->height = tex_height;

	//頂点バッファに反映
	TransferVertices();
}

XMFLOAT2 Sprite::Getposition()
{
	return XMFLOAT2(position.m128_f32[0],position.m128_f32[1]);
}

void Sprite::Fadein(const std::string& swich)
{
	if (swich == "ON")
	{
		Color.w += 0.01f;
	}
	
	if (Color.w >= 1.0f)
	{
		Color.w = 1.0f;
	}
}

void Sprite::Fadeout(const std::string& swich)
{
	if (swich == "ON")
	{
		Color.w -= 0.007f;
	}

	if (Color.w <= 0.0f)
	{
		Color.w = 0.0f;
	}
}

void Sprite::FadeOut()
{
	const float decreaseValue = 0.005f;//減少値
	if (fadeOutFlag)
	{
		time -= decreaseValue;

		if (time < 0)
		{
			time = limit;
			ChengeFadeOutFlag();
		}
	}
}

void Sprite::Common::InitializeGraphicsPipeline()
{
	HRESULT result = S_FALSE;

	const int spriteSRVCount = 512;//テクスチャの最大枚数

	//デスクリプタレンジ
	CD3DX12_DESCRIPTOR_RANGE descRangeSRV;
	descRangeSRV.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);//t0　レジスタ 

	//ルートパラメーター
	CD3DX12_ROOT_PARAMETER rootparam[2];
	rootparam[0].InitAsConstantBufferView(0, 0, D3D12_SHADER_VISIBILITY_ALL);//定数バッファビューとして初期化(b0)
	rootparam[1].InitAsDescriptorTable(1, &descRangeSRV, D3D12_SHADER_VISIBILITY_ALL);

	CD3DX12_STATIC_SAMPLER_DESC samplerDesc = CD3DX12_STATIC_SAMPLER_DESC(0);

	//各シェーダファイルの読み込みとコンパイル
	ID3DBlob* vsBlob = nullptr; // 頂点シェーダオブジェクト
	ID3DBlob* psBlob = nullptr; // ピクセルシェーダオブジェクト
	ID3DBlob* errorBlob = nullptr; // エラーオブジェクト
	// 頂点シェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpriteVertexShader.hlsl", // シェーダファイル名
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE, // インクルード可能にする
		"VSmain", "vs_5_0", // エントリーポイント名、シェーダーモデル指定
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION, // デバッグ用設定
		0,
		&vsBlob, &errorBlob);
	// ピクセルシェーダの読み込みとコンパイル
	result = D3DCompileFromFile(
		L"Resources/Shaders/SpritePixelShader.hlsl", // シェーダファイル名
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
		errstr += "¥n";
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
	gpipeline.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);//一旦標準値をセット
	gpipeline.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;         //背面カリングをしない

	//標準的な設定(深度設定を行う,書き込み許可、深度が小さければ合格)
	gpipeline.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);//一旦標準値をセット　
	gpipeline.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;   //常に上書きルール       

	//レンダーターゲットのブレンド設定(8個あるがいまは1つしか使わない)
	D3D12_RENDER_TARGET_BLEND_DESC blenddesc{};
	blenddesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	//共通設定
	blenddesc.BlendEnable = true;
	blenddesc.BlendOp = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blenddesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;

	blenddesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blenddesc.SrcBlendAlpha = D3D12_BLEND_ONE;
	blenddesc.DestBlendAlpha = D3D12_BLEND_ZERO;

	//ブレンドステートに設定する
	gpipeline.BlendState.RenderTarget[0] = blenddesc;

	//深度バッファのフォーマット
	gpipeline.DSVFormat = DXGI_FORMAT_D32_FLOAT;//深度値フォーマット

	//頂点ステートの設定
	gpipeline.InputLayout.pInputElementDescs = inputLayout;
	gpipeline.InputLayout.NumElements = _countof(inputLayout);

	//図形の形状を三角形に設定
	gpipeline.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

	//その他の設定
	gpipeline.NumRenderTargets = 1; // 描画対象は 1 つ
	gpipeline.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // 0~255 指定の RGBA
	gpipeline.SampleDesc.Count = 1; // 1 ピクセルにつき 1 回サンプリング

	//ルートシグネチャの設定
	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init_1_0(_countof(rootparam), rootparam, 1, &samplerDesc,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> rootSigBlob;
	//バージョン自動判定でのシリアライズ
	result = D3DX12SerializeVersionedRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1_0,
		&rootSigBlob, &errorBlob);

	////ルートシグネチャの生成
	result = dxBase->GetDevice()->CreateRootSignature(0, rootSigBlob->GetBufferPointer(), rootSigBlob->GetBufferSize(),
		IID_PPV_ARGS(&RootSignature));
	if (FAILED(result))
	{
		assert(0);
	}

	//グラフィックスパイプラインステートの設定にセット
	gpipeline.pRootSignature = RootSignature.Get();

	//パイプラインにルートシグネチャをセット
	result = dxBase->GetDevice()->CreateGraphicsPipelineState(&gpipeline, IID_PPV_ARGS(&PipelineState));

	if (FAILED(result))
	{
		assert(0);
	}
}