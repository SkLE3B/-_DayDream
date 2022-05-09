cbuffer cbuff0 : register(b0)
{
	matrix inverse;//ビュー逆行列
	matrix viewproj;//ビュープロジェクション行列
	matrix world; // ワールド行列
	float4 Animation;
	float4 Time;
	float  FadeOut;
};

//頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos :SV_POSITION;//システム用頂点座標
	float2 uv :TEXCOORD;   //uv値
};