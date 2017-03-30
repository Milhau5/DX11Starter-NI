//only one thread
//main will be empty
[numthreads(1, 1, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
}

//Constants
static const float Pi = 3.141592654f;
static const float Pi2 = 6.283185307f;
static const float Pi_2 = 1.570796327f;
static const float Pi_4 = 0.7853981635f;
static const float InvPi = 0.318309886f;
static const float InvPi2 = 0.159154943f;

//Sampler States
SamplerState SamplerLinear
{
	Filter = MIN_MAG_MIP_LINEAR;
	AddressU = Wrap;
	AddressV = Wrap;
};

SamplerState SamplerAnisotropic
{
	Filter = ANISOTROPIC;
	MaxAnisotropy = 16;
	AddressU = Wrap;
	AddressV = Wrap;
};

//Hammersly Points in 2D (point set on a hemisphere)
//http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
//code converted from GLSL
float2 Hammersley(uint i, uint N)
{
	float ri = reversebits(i) * 2.3283064365386963e-10f; //reversebits() is a HLSL method
	return float2(float(i) / float(N), ri);
}

//Specular BRDF
//Step 1: Specular Distribution Term, using Trowbridge-Reitz (GGX)
//http://graphicrants.blogspot.com.au/2013/08/specular-brdf-reference.html
float GGX(float NdotV, float a)
{
	float k = a / 2;
	return NdotV / (NdotV * (1.0f - k) + k);
}

//Step 2: Geometric Shadowing Term
// Defines the shadowing from the microfacets.
//
// Smith approximation:
// http://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf
// http://graphicrants.blogspot.fr/2013/08/specular-brdf-reference.html
//
// ===============================================================================================
float G_Smith(float a, float nDotV, float nDotL)
{
	return GGX(nDotL, a * a) * GGX(nDotV, a * a);
}

//Step 3: Fresnel Term
// The Fresnel function describes the amount of light that reflects from a mirror surface 
// given its index of refraction. 
//
// Schlick's approximation:
// http://blog.selfshadow.com/publications/s2013-shading-course/rad/s2013_pbs_rad_notes.pdf
// http://graphicrants.blogspot.fr/2013/08/specular-brdf-reference.html
//
// ================================================================================================
float3 Schlick_Fresnel(float3 f0, float3 h, float3 l)
{
	return f0 + (1.0f - f0) * pow((1.0f - dot(l, h)), 5.0f);
}

//In order to do Image-based lighting, we need to solve the Radiance Integral
//This is done via importance sampling, as outline below:
// http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf 
//tl;dr --> We need a PMREM for indirect specular, which means we sample environment map hundreds of thousands of times...
//...so, we split the map into important sections and sample those sections. See PixelShader.
float3 ImportanceSampleGGX(float2 Xi, float Roughness, float3 N)
{
	float a = Roughness * Roughness; // DISNEY'S ROUGHNESS [see Burley'12 siggraph]

	float Phi = 2 * Pi * Xi.x;
	float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
	float SinTheta = sqrt(1 - CosTheta * CosTheta);

	float3 H;
	H.x = SinTheta * cos(Phi);
	H.y = SinTheta * sin(Phi);
	H.z = CosTheta;

	float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
	float3 TangentX = normalize(cross(UpVector, N));
	float3 TangentY = cross(N, TangentX);

	// Tangent to world space
	return TangentX * H.x + TangentY * H.y + N * H.z;
}