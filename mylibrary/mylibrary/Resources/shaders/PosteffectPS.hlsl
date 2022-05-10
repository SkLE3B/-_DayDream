#include "PostEffect.hlsli"

Texture2D<float> tex : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex0 : register(t1);

SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

float WhiteNoise(float2 coord) {
	return frac(sin(dot(coord, float2(8.7819, 3.255))) * 437.645);
}

float4 main(VSOutput input) : SV_TARGET
{
	float4 texcolor = tex.Sample(smp,input.uv);

	//return float4(texcolor.rgb, 1);

	//モザイク
	//float density = 70;
	//return tex.Sample(smp, floor(input.uv * density) / density);

	//////色収差
	//float shift = 0.005f;
	//float r = tex.Sample(smp, input.uv + float2(-shift,0)).r;
	//float g = tex.Sample(smp, input.uv + float2(0, 0)).g;
	//float b = tex.Sample(smp, input.uv + float2(shift, 0)).b;
	//return float4(r, g, b, 1);

	//セピア
	//float sepia = 0.3f;
	//float grayScale = texcolor.r * 0.299 + texcolor.g * 0.587 + texcolor.b * 0.114;
	//return float4(grayScale + sepia, grayScale, grayScale - sepia, 1);

	//ネガポジ
	//return float4(1 - texcolor.r, 1 - texcolor.g, 1 - texcolor.b * 3, 1);
	//return float4(texcolor.rgb,1);

	//ホワイトノイズ
	//float2 samplePoint = input.uv;
	//float4 Tex = tex.Sample(smp, samplePoint);
	//float noise = WhiteNoise(input.uv * Time) - 0.5;
	//Tex.rgb += float3(noise, noise, noise);
	//return Tex;

	//走査線
	float2 samplePoint = input.uv;
	float4 Tex = tex.Sample(smp, samplePoint);
	float sinv = sin(input.uv.y * 2 + Time * -0.1);
	float steped = step(0.99, sinv * sinv);
	Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 50.0 + Time * 1.0)) * 0.05;
	Tex.rgb -= (1 - steped) * abs(sin(input.uv.y * 100.0 - Time * 2.0)) * 0.08;
	Tex.rgb += steped * 0.1;
	return Tex * FadeOut;

	////樽状湾曲
	//float2 samplePoint = input.uv;
	//samplePoint -= float2(0.5, 0.5);
	//float distPower = pow(length(samplePoint), 0.5);
	//samplePoint *= float2(distPower, distPower);
	//samplePoint += float2(0.5, 0.5);
	//float4 Tex = tex.Sample(smp, samplePoint);
	//return Tex;

	//ビネット
	//float2 samplePoint = input.uv;
	//float4 Tex = tex.Sample(smp, samplePoint);
	//float vignette = length(float2(0.5, 0.5) - input.uv);
	//vignette = clamp(vignette - 0.1, 0, 1);
	//Tex.rgb -= vignette;
	//return Tex;

	//グリッジ
	//float vertNoise = WhiteNoise(float2(floor((input.uv.x) / 5.0) * 5.0, Time * 0.1));
	//float horzNoise = WhiteNoise(float2(floor((input.uv.y) / 3.0) * 3.0, Time * 0.2));
	//float vertGlitchStrength = vertNoise / 1.0;
	//float horzGlitchStrength = horzNoise / 1.0;
	//vertGlitchStrength = vertGlitchStrength * 2.0 - 1.0;
	//horzGlitchStrength = horzGlitchStrength * 2.0 - 1.0;
	//float V = step(vertNoise, 3.0 * 2) * vertGlitchStrength;
	//float H = step(horzNoise, 3.0) * horzGlitchStrength;

	//float2 samplePoint = input.uv;
	//float sinv = sin(samplePoint.y * 2 - Time * -0.1);
	//float steped = 1 - step(0.99, sinv * sinv);
	//float timeFrac = steped * step(0.8, frac(Time));
	//samplePoint.x += timeFrac * (V + H);
	//float4 Tex = tex.Sample(smp, samplePoint);
	//return Tex;
}