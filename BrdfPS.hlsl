#include "CommonVars.hlsl"

struct PSInput
{
	float4 Position			: SV_Position;
	float2 TexCoord			: TEXCOORD0;
};

TextureCube Cubemap : register(t0);

// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf 
float2 IntegrateBRDF(float Roughness, float NoV)
{
	float3 V;

	V.x = sqrt(1.0f - NoV * NoV);	// Sine
	V.y = 0;
	V.z = NoV;						// Cosine

	float A = 0;
	float B = 0;

	float3 N = float3(0.0f, 0.0f, 1.0f);

	const uint NumSamples = 1024;

	for (uint i = 0; i < NumSamples; i++)
	{
		float2 Xi = Hammersley(i, NumSamples);
		float3 H = ImportanceSampleGGX(Xi, Roughness, N);
		float3 L = 2.0f * dot(V, H) * H - V;

		float NoL = saturate(L.z);
		float NoH = saturate(H.z);
		float VoH = saturate(dot(V, H));

		if (NoL > 0)
		{
			float G = G_Smith(Roughness, NoV, NoL);
			float G_Vis = G * VoH / (NoH * NoV);

			float Fc = pow(1 - VoH, 5);

			A += (1 - Fc) * G_Vis;
			B += Fc * G_Vis;
		}
	}

	return float2(A, B) / NumSamples;
}

// Pixel Shader (the entry point)
float4 main(PSInput input) : SV_TARGET
{
	return float4(IntegrateBRDF(input.TexCoord.x, input.TexCoord.y), 0, 1);
}