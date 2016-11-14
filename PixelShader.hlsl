
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float4 worldSpace   : TEXTCOORD1;
	float3 normal       : NORMAL;
	//float3 worldPos     : POSITION;
	//float3 tangent      : TANGENT;
	float2 uv           : TEXCOORD;
};

//Globals
Texture2D diffuseTexture   : register(t0);
SamplerState basicSampler  : register(s0);

// External texture-related data
//Texture2D Texture		: register(t0);
//Texture2D NormalMap		: register(t1);
//TextureCube Sky			: register(t2);
//SamplerState Sampler	: register(s0);

//A new directional light
//we don't need semantics
struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
};

// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalLight : register(b0)
{
	DirectionalLight light;
	DirectionalLight newLight;

	//new additions
	//float3 CameraPosition;
};

//we would want to eventually use this instead of making temp values (see main() below)
/*cbuffer fogVariables : register(c0)
{
	const float FogEnabled;
	const float FogStart;
	const float FogEnd;
	const float FogColor;
};*/

//lets see what we can do with this
/*float ComputeFogFactor(float d) {
	//d is the distance to the geometry sampling from the camera
	//this simply returns a value that interpolates from 0 to 1 
	//with 0 starting at FogStart and 1 at FogEnd 
	return clamp((d - FogStart) / (FogEnd - FogStart), 0, 1) * FogEnabled;
}*/

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	float3 updated = normalize(input.normal);
	input.normal = updated;

	//fog-related stuff
	float dist = 0;
	float fogFactor = 0;
	float4 fogColor = float4(0.5, 0.5, 0.5, 1.0); //grey

	//range-based
	dist = length(input.worldSpace); //mag

	//linear fog
	fogFactor = (10 - dist) / (10 - 5);
	fogFactor = clamp(fogFactor, 0.0, 1.0);

	//sample the texture
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	//Normalized direction TO the light
	float3 goTowardsTheLight = -normalize(light.Direction);
	//Light amount
	float dProduct = saturate(dot(input.normal, goTowardsTheLight)); //used to be a float4
	//Return final surface color
	float4 finalResult = (light.DiffuseColor * dProduct * surfaceColor) + (light.AmbientColor * surfaceColor);

	//Normalized direction TO the light
	float3 goToLight = -normalize(newLight.Direction);
	//Light amount
	float amount = saturate(dot(input.normal, goToLight)); //used to be a float4
	//Return final surface color
	float4 nextResult = (newLight.DiffuseColor * amount * surfaceColor) + (newLight.AmbientColor * surfaceColor);

	//return finalResult + nextResult;
	float4 tempResult = finalResult + nextResult;

	float4 finalColor = lerp(fogColor, tempResult, fogFactor);
	return finalColor;
	
}