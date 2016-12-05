
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
	float4 posForShadow : TEXCOORD1;
	float4 position		: SV_POSITION;
	//float4 worldSpace   : TEXCOORD1;
	float3 normal       : NORMAL;
	//float4 worldPos     : POSITION;
	float2 uv           : TEXCOORD;
};

//Globals
TextureCube Sky            : register(t0);
Texture2D diffuseTexture   : register(t1);
Texture2D ShadowMap        : register(t2);
SamplerState basicSampler  : register(s0);
SamplerComparisonState ShadowSampler  : register(s1);

//A new directional light
//we don't need semantics
struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
	//float nope;
};

//NEW light that needs its own shadowing
/*struct SpotLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Position;
	float DiffuseIntensity;
	float3 Direction;
	float nope;
};*/

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
	//SpotLight spotLight;

	//new additions
	float3 CameraPosition;
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

//Nice read!
//http://gamedev.stackexchange.com/questions/56897/glsl-light-attenuation-color-and-intensity-formula
/*float4 calcSpotLight(float4 worldPos, float3 normal, SpotLight spotLight)
{
	//multiply attenuation by intensity
	//attenuation function: 1.0 / (1.0 + a*dist + b*dist*dist))
	//float a = 5;
	//float b = 4;

	float3 spotLightDirectionToPixel = worldPos - spotLight.Position;
	//float dist = sqrt(dot(pointLightDirection, pointLightDirection));
	//dist = max(dist, 9);
	//float attenuation = 1.0 / (1.0 + a*dist + b*dist*dist);
	spotLightDirectionToPixel = -normalize(spotLightDirectionToPixel);

	//float temp = attenuation * pointLight.Intensity;

	//N dot L
	float lightAmount = dot(normal, spotLightDirectionToPixel);
	lightAmount = saturate(lightAmount);

	//why 
	float angleFromCenter = max(0.5f, dot(spotLightDirectionToPixel, spotLight.Direction));
	//raise to a power for a nice "falloff"
	//multiply diffuse and specular results by this
	float spotAmount = pow(angleFromCenter, 0.7f);
	//float spotAmount = smoothstep(100.0f, 100.0f, angleFromCenter);
	//float spotAmount = 1.0f;

	float4 scaledDiffuse = spotLight.DiffuseColor * lightAmount * spotAmount;


	//return float4(spotAmount, 0, 0, 0);
	return scaledDiffuse + spotLight.AmbientColor;
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
	/*float dist = 0;
	float fogFactor = 0;
	float4 fogColor = float4(0.5, 0.5, 0.5, 1.0); //grey

	//range-based
	dist = length(input.worldSpace); //mag

	//linear fog
	fogFactor = (10 - dist) / (10 - 5);
	fogFactor = clamp(fogFactor, 0.0, 1.0);*/

	//sample the texture
	float4 surfaceColor = diffuseTexture.Sample(basicSampler, input.uv);

	//NEW
	//float4 spotL = calcSpotLight(input.worldPos, input.normal, spotLight);

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

	// Shadow map calculation
	// Figure out this pixel's UV in the SHADOW MAP
	float2 shadowUV = input.posForShadow.xy / input.posForShadow.w * 0.5f + 0.5f;
	shadowUV.y = 1.0f - shadowUV.y; // Flip the Y since UV coords and screen coords are different

	// Calculate this pixel's actual depth from the light
	float depthFromLight = input.posForShadow.z / input.posForShadow.w;

	// Sample the shadow map
	float shadowAmount = ShadowMap.SampleCmpLevelZero(ShadowSampler, shadowUV, depthFromLight);

	//return finalResult + nextResult;
	float4 tempResult = finalResult + nextResult;

	//float4 finalColor = lerp(fogColor, tempResult, fogFactor);
	//return finalColor;
	
	return tempResult * shadowAmount;

	//return spotL;
	
}