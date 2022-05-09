cbuffer cbuff0 : register(b0)
{
	float Time;
	float FadeOut;
};

//頂点シェーダーからピクセルシェーダーへのやり取りに使用する構造体
struct VSOutput
{
	float4 svpos :SV_POSITION;//システム用頂点座標
	float2 uv :TEXCOORD;   //uv値
	float4 worldpos : POSITION;//ワールド座標
	float4 distance : DISTANCE ;
};