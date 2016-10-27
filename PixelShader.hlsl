
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
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
	//float4 color		: COLOR;
};

//Globals
Texture2D diffuseTexture   : register(t0);
SamplerState basicSampler  : register(s0);

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
};

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

	return finalResult + nextResult;
	//float3 finalColor = (light.DiffuseColor * dProduct) + (newLight.DiffuseColor * amount);
	//return float4(finalColor, 1);
}