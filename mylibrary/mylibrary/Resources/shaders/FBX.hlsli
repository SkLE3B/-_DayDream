//ボーンの最大値
static const int MAX_BONES = 32;

cbuffer cbuff0 : register(b0)
{
	matrix viewproj;//ビュープロジェクション	
	matrix world;// ワールド行列
	float3 cameraPos;//カメラ座標(ワールド座標)
	float4 color;// 色(RGBA)
};

cbuffer skining : register(b3)//ボーンのスキニング行列が入る
{
	matrix matSkinning[MAX_BONES];
};

//バーテックスバッファーの入力
struct VSInput
{
	float4 pos	: POSITION;//位置
	float3 normal : NORMAL;//頂点座標
	float2 uv	: TEXCOORD;//テクスチャ座標
	uint4  boneIndices : BONEINDICES;//ボーン番号
	float4 boneWeights : BONEWEIGHTS;//ボーンのスキンウェイト　重み
};

//頂点シェーダーからピクセルシェーダーのやり取りに使用する
//構造体
struct VSOutput
{
	float4 svpos : SV_POSITION;//システム用頂点座標
	float3 normal : NORMAL;//法線
	float2 uv : TEXCOORD; //uv値
};