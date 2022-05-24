#include "MainFog.hlsli"

Texture2D<float4> tex : register(t0);  // 0番スロットに設定されたテクスチャ
Texture2D<float4> tex0 : register(t1); //深度バッファテクスチャ

SamplerState smp : register(s0);      // 0番スロットに設定されたサンプラー

//ランダムベクトル取得
float2 randomVec(float2 fact)
{
	float2 angle = float2(
		dot(fact, float2(127.1f, 311.7f)),
		dot(fact, float2(269.5f, 183.3f))
		);

	return frac(sin(angle) * 43758.5453123) * 2 - 1;
}

//ノイズの密度をdensityで設定,uvにi.uvを代入  
float PerlinNoise(float density, float2 uv)
{
	float2 _ScreenParams = float2(1280, 720);
	float2 uvFloor = floor(uv * density * _ScreenParams.xy);
	float2 uvFrac = frac(uv * density * _ScreenParams.xy);

	float2 v00 = randomVec(uvFloor + float2(0, 0));//各頂点のランダムなベクトルを取得
	float2 v01 = randomVec(uvFloor + float2(0, 1));
	float2 v10 = randomVec(uvFloor + float2(1, 0));
	float2 v11 = randomVec(uvFloor + float2(1, 1));

	float c00 = dot(v00, uvFrac - float2(0, 0));//内積を取る
	float c01 = dot(v01, uvFrac - float2(0, 1));
	float c10 = dot(v10, uvFrac - float2(1, 0));
	float c11 = dot(v11, uvFrac - float2(1, 1));

	float2 u = uvFrac * uvFrac * (3 - 2 * uvFrac);

	float v0010 = lerp(c00, c10, u.x);
	float v0111 = lerp(c01, c11, u.x);

	return lerp(v0010, v0111, u.y) / 2 + 0.5f;
}

//フラクタル和
float FractalSumNoise(float density, float2 uv)
{
	float fn;
	fn  = PerlinNoise(density * 1, uv) * 1.0f /  2;
	fn += PerlinNoise(density * 2, uv) * 1.0f /  4;
	fn += PerlinNoise(density * 4, uv) * 1.0f /  8;
	fn += PerlinNoise(density * 8, uv) * 1.0f / 16;

	return fn;
}

float RayleighPhaseFunction(float fCos2)
{
	return 3.0f / (16.0f * 3.1435) * (1.0f + fCos2);
}

float MiePhaseFunction(float g, float fCos)
{
	return (1 - g) * (1 - g) / (4.0f * 3.1435 * pow(1 + g * g - 2.0f * g * fCos, 1.5f));
}

float4 main(VSOutput input) : SV_TARGET
{
	 float4 texcolor = tex.Sample(smp,input.uv);
	 ////定数フォグ
	 float depth = tex0.Sample(smp, input.uv);
	 //正規デバイス座標系での座標
	 float4 viewPos = float4(0, 0, depth, 1);
	 //プロジェクション逆行列
	 viewPos = mul(inverse, viewPos);
	 //W除算
	 viewPos.z /= viewPos.w;

	 float fogWeight = 0.0f;
	 float fog_Scale = 1.0f;
	 //霧減数率
	 float g = 0.04f;
	 fogWeight += fog_Scale * max(0.0f, 1.0f - exp(-g * viewPos.z));
	 float4 bgColor = tex.Sample(smp, input.uv);

	 float density = 0.002f;
	 float timeX =  Time.x;
	 float timeY = Time.y;

	 //右方向のベクトル
	 float toTheRight = FractalSumNoise(density, input.uv + float2(timeX, timeY));
	 
	 //右斜め下方向のベクトル
	 float diagonallyDownwardToTheRight = FractalSumNoise(density, input.uv + float2(timeX, timeX));
	
	 float  noise = toTheRight + diagonallyDownwardToTheRight * fogWeight;
	 
	 float sunCos = -0.14f;
	 //float sunCos = float4(0.0f,-0.0f,0.0f,1.0f);
	 const float betaPhaseR = RayleighPhaseFunction(sunCos * sunCos);
	 const float betaPhaseM = MiePhaseFunction(-0.75, sunCos);
	 float rayleighCoeff = 0.8f;
	 float4 rayleighColor = float4(1, 0, 0, 1);
	 float mieCoeff = 1.0f;
	 float4 mieColor = float4(0, 0, 1, 1);
	 float4 fogColor = (betaPhaseR * rayleighCoeff * rayleighColor + betaPhaseM * mieCoeff * mieColor) / (rayleighCoeff + mieCoeff);
	 //float4 fogColor = float4(noise, noise, noise, 1);

	 float4 outputColor = lerp(bgColor, fogColor + noise, fogWeight);
	 return outputColor * FadeOut;
}