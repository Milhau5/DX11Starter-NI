#include "CommonVars.hlsl"

cbuffer PSConstants: register(b0)
{
	float2 halfPixel;
	float Face;
	float MipIndex;
};

struct PSInput //like all PS, match output of VS
{
	float4 Position			: SV_Position;
	float2 TexCoord			: TEXCOORD0;
};

TextureCube cubeMap  :  register(t0);

float3 GetNormal(uint face, float2 uv)
{
	float2 debiased = uv * 2.0f - 1.0f;

	float3 dir = 0;

	//where is this particular face...well, facing
	switch (face)
	{
	case 0: dir = float3(1, -debiased.y, -debiased.x);
		break;

	case 1: dir = float3(-1, -debiased.y, debiased.x);
		break;

	case 2: dir = float3(debiased.x, 1, debiased.y);
		break;

	case 3: dir = float3(debiased.x, -1, -debiased.y);
		break;

	case 4: dir = float3(debiased.x, -debiased.y, 1);
		break;

	case 5: dir = float3(-debiased.x, -debiased.y, -1);
		break;
	};

	return normalize(dir);
}

//We solve the radiance integral using the Split-Sum approximation
//This will solve the first of two sums, by Pre-Filtering the Environment Map
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf 
//^^Pages 5-6^^
float3 PrefilterEnvMap(float Roughness, float3 R, TextureCube EnvMap)
{
	float TotalWeight = 0.0000001f;

	//assume viewing angle to the surface is 0
	float3 N = R;
	float3 V = R;
	float3 PrefilteredColor = 0;

	const uint NumSamples = 1024;

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, Roughness, N);
		float3 L = 2 * dot(V, H) * H - V;
		float NoL = saturate(dot(N, L));

		if (NoL > 0)
		{
			PrefilteredColor += EnvMap.SampleLevel(SamplerAnisotropic, L, 0).rgb * NoL;
			TotalWeight += NoL;
		}
	}

	return PrefilteredColor / TotalWeight;
}

float4 main(PSInput input) : SV_TARGET
{
	//float3 normal = GetNormal(Face, input.TexCoord);
	float  roughness = saturate(MipIndex / 6.0f); // Mip level is in [0, 6] range and roughness is [0, 1]

	return float4(PrefilterEnvMap(roughness, 1.0, cubeMap), 1);
}