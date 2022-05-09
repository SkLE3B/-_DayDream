#include "FBX.hlsli"
//0番スロットに設定されたテクスチャ
Texture2D<float4> tex : register(t0);
//0番スロットに設定されたサンプラー
SamplerState smp : register(s0);
//エントリーポイント
float4 main(VSOutput input) : SV_TARGET
{
	//トゥーンシェーダー
	float3 light = normalize(float3(1, 1, 1)); // 右下奥　向きのライト
	float diffuse = saturate(dot(-light, input.normal));
	float specular = pow(diffuse, 100);

	if (diffuse > 0.75f)
	{
		diffuse = 1.0f;
	}
	else  if (diffuse > 0.5f)
	{
		diffuse = 0.5f;
	}
	else if(diffuse > 0.35f)
	{
		diffuse = 0.25f;
	}
	else
	{
		diffuse = 0.20f;
	}

	float4 brightness = diffuse + 0.3f;
	float4 texcolor = tex.Sample(smp, input.uv);
	return float4(texcolor.rgb * brightness + float3(specular, specular, specular), texcolor.a);
}