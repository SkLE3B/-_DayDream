#include "MainFog.hlsli"

Texture2D<float4> tex : register(t0);  // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
Texture2D<float4> tex0 : register(t1); //�[�x�o�b�t�@�e�N�X�`��

SamplerState smp : register(s0);      // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

//�����_���x�N�g���擾
float2 randomVec(float2 fact)
{
	float2 angle = float2(
		dot(fact, float2(127.1f, 311.7f)),
		dot(fact, float2(269.5f, 183.3f))
		);

	return frac(sin(angle) * 43758.5453123) * 2 - 1;
}

//�m�C�Y�̖��x��density�Őݒ�,uv��i.uv����  
float PerlinNoise(float density, float2 uv)
{
	float2 _ScreenParams = float2(1280, 720);
	float2 uvFloor = floor(uv * density * _ScreenParams.xy);
	float2 uvFrac = frac(uv * density * _ScreenParams.xy);

	float2 v00 = randomVec(uvFloor + float2(0, 0));//�e���_�̃����_���ȃx�N�g�����擾
	float2 v01 = randomVec(uvFloor + float2(0, 1));
	float2 v10 = randomVec(uvFloor + float2(1, 0));
	float2 v11 = randomVec(uvFloor + float2(1, 1));

	float c00 = dot(v00, uvFrac - float2(0, 0));//���ς����
	float c01 = dot(v01, uvFrac - float2(0, 1));
	float c10 = dot(v10, uvFrac - float2(1, 0));
	float c11 = dot(v11, uvFrac - float2(1, 1));

	float2 u = uvFrac * uvFrac * (3 - 2 * uvFrac);

	float v0010 = lerp(c00, c10, u.x);
	float v0111 = lerp(c01, c11, u.x);

	return lerp(v0010, v0111, u.y) / 2 + 0.5f;
}

//�t���N�^���a
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
	return 3.0f / (16.0f * 3.1415926535) * (1.0f + fCos2);
}

float MiePhaseFunction(float g, float fCos)
{
	return (1 - g) * (1 - g) / (4.0f * 3.1415926535 * pow(1 + g*g - 2.0f*g*fCos, 1.5f));
}

float4 main(VSOutput input) : SV_TARGET
{
	 ////�萔�t�H�O
	 float depth = tex0.Sample(smp, input.uv);
	 //���K�f�o�C�X���W�n�ł̍��W
	 float4 viewPos = float4(0, 0, depth, 1);
	 //�v���W�F�N�V�����t�s��
	 viewPos = mul(inverse, viewPos);
	 //W���Z
	 viewPos.z /= viewPos.w;

	 float fogWeight = 0.0f;
	 //���̔Z��
	 float fog_Scale = 1.0f;
	 //��������(�J�����Ƃ̉e������)
	 //float g = 0.004f;
	 float g = 0.02f;

	 fogWeight += fog_Scale * max(0.0f, 1.0f - exp(-g * viewPos.z));
	 float4 bgColor = tex.Sample(smp, input.uv);

	 //�m�C�Y�̖��x
	 float density = 0.002f;
	 float timeX =  Time.x;
	 float timeY = Time.y;

	 //�E�����̃x�N�g��
	 float cos = 1.0f - viewPos.z;
	 float toTheRight = FractalSumNoise(density, input.uv + float2(timeX, timeY));
	 float4 noise = float4(toTheRight, toTheRight, toTheRight, 1.0f);
	 float4 sunCos = 2.0f;
	 float valueG = -0.75f;
	 const float betaPhaseR = RayleighPhaseFunction(sunCos * sunCos);
	 const float betaPhaseM = MiePhaseFunction(valueG, sunCos);
	 float rayleighCoeff = 0.025f;
	 float4 rayleighColor = float4(1, 1, 1, 1);
	 float mieCoeff = 0.025f;
	 float4 mieColor = float4(1, 1, 1, 1);
	 float4 fogColor = (betaPhaseR * rayleighCoeff * rayleighColor + betaPhaseM * mieCoeff * mieColor) / (rayleighCoeff + mieCoeff);
	 float4 outputColor = lerp(bgColor, fogColor + noise, fogWeight);
	
	 return outputColor * FadeOut;
}