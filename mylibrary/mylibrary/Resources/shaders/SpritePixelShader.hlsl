#include "SpriteShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);//0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); //0番スロットに設定されたサンプラー

float4 PSmain(VSOutput input) : SV_TARGET
{	
	////ビネット
	float2 samplePoint = input.uv;
	float4 Tex2 = tex.Sample(smp, samplePoint);
	float vignette = length(float2(0.5, 0.5) - input.uv);
	vignette = clamp(vignette - 0.3, 0, 0.8);
	Tex2.rgb -= vignette;
	return Tex2 * Time * color;
}