// Constant Buffer for external (C++) data
cbuffer externalData : register(b0)
{
	matrix world; //for current entity
	matrix view; //for light's POV
	matrix projection; //for light's POV
};

// Struct representing a single vertex worth of data
struct VertexShaderInput
{
	float3 position		: POSITION;
};

// Only thing we need to output for shadow map creation
struct VertexToPixel
{
	float4 position		: SV_POSITION;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// --------------------------------------------------------
VertexToPixel main(VertexShaderInput input)
{
	// Set up output
	VertexToPixel output;

	// Calculate output position
	matrix worldViewProj = mul(mul(world, view), projection);
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	return output;
}