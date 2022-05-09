#include "OBJShaderHeader.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float4 main(VSOutput input) : SV_TARGET
{
	float3 light = normalize(float3(1,-1,1)); // 右下奥　向きのライト
	float light_diffuse = saturate(dot(-light, input.normal));
	float3 shade_color;
	shade_color = m_ambient; // アンビエント項
	shade_color += m_diffuse * light_diffuse;	// ディフューズ項
	float4 texcolor = tex.Sample(smp, input.uv);
	return float4(texcolor.rgb * shade_color, texcolor.a * m_alpha);

	//トゥーンシェーダー
	//float3 light = normalize(float3(1, -1, -1)); // 右下奥　向きのライト
	//float diffuse = saturate(dot(-light, input.normal));
	//float specular = pow(diffuse, 100);

	//specular = specular * 0.5f + 0.5f;
	//specular = specular * specular;

	//float t = 0.1f;
	//float g = 0.13f;

	//float4 b = smoothstep(t, g, diffuse) * float4(1.0f, 1.0f, 1.0f, 1.0f);
	//float4 d = (1 - smoothstep(t, g, diffuse)) * float4(0.3f, 0.3f, 0.3f, 1.0f);
	//float4 toon = b + d;

	//if (diffuse > 0.5f)
	//{
	//	diffuse = 1.0f;
	//}
	//else
	//{
	//	diffuse = 0.0f;
	//}

	//float brightness = toon + 0.3f;
	//float4 texcolor = tex.Sample(smp, input.uv);
	//return float4(texcolor.rgb * brightness + float3(specular, specular, specular), texcolor.a);
}