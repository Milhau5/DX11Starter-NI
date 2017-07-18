
// Constant Buffer
// - Allows us to define a buffer of individual variables 
//    which will (eventually) hold data from our C++ code
// - All non-pipeline variables that get their values from 
//    our C++ code must be defined inside a Constant Buffer
// - The name of the cbuffer itself is unimportant
cbuffer externalData : register(b0)
{
	matrix world;
	matrix view;
	matrix projection;

	matrix shadowView;
	matrix shadowProjection;
};

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{ 
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 position		: POSITION;     // XYZ position
	float3 normal       : NORMAL;
	float2 uv           : TEXCOORD;
	//float3 tangent      : TANGENT;
};

// Struct representing the data we're sending down the pipeline
// - Should match our pixel shader's input (hence the name: Vertex to Pixel)
// - At a minimum, we need a piece of data defined tagged as SV_POSITION
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 posForShadow : TEXCOORD0;
	//may need a "dirForShadow" for Spot Light
	float4 position		: SV_POSITION;	// XYZW position (System Value Position)
	float4 worldSpace   : TEXCOORD1; //fog
	float3 normal       : NORMAL;
	float3 positionWS   : POSITION; //the world position as a float3
	//float4 worldPos     : POSITION; //may be better than worldSpace someday. REMEMBER TO CHANGE BACK TO FLOAT3 later
	float2 uv           : TEXCOORD2;
};

// --------------------------------------------------------
// The entry point (main method) for our vertex shader
// 
// - Input is exactly one vertex worth of data (defined by a struct)
// - Output is a single struct of data to pass down the pipeline
// - Named "main" because that's the default the shader compiler looks for
// --------------------------------------------------------
VertexToPixel main( VertexShaderInput input )
{
	// Set up output struct
	VertexToPixel output;

	// The vertex's position (input.position) must be converted to world space,
	// then camera space (relative to our 3D camera), then to proper homogenous 
	// screen-space coordinates.  This is taken care of by our world, view and
	// projection matrices.  
	//
	// First we multiply them together to get a single matrix which represents
	// all of those transformations (world to view to projection space)
	matrix worldViewProj = mul(mul(world, view), projection);
	float4 worldPositionTemp = mul(input.position, world); //lets just take this part

	output.worldSpace = mul(float4(input.position, 1.0f), mul(world, view)); //this is the only line changed in the math

	// Then we convert our 3-component position vector to a 4-component vector
	// and multiply it by our final 4x4 matrix.
	//
	// The result is essentially the position (XY) of the vertex on our 2D 
	// screen and the distance (Z) from the camera (the "depth" of the pixel)
	output.position = mul(float4(input.position, 1.0f), worldViewProj);

	//useful for us (now) because shapes are on a uniform scale
	output.normal = mul(input.normal, (float3x3)world);
	output.positionWS = worldPositionTemp.xyz; //this is going into the Cook-Torrence Microfacet BRDF

	//tan
	//output.tangent = mul(input.tangent, (float3x3)world); // Needed for normal mapping

    // Get world position of vertex
	//output.worldPos = mul(float4(input.position, 1.0f), world); //used to end with.xyz

	//UVs
	output.uv = input.uv;

	// Do shadow map calc
	matrix shadowWVP = mul(mul(world, shadowView), shadowProjection);
	output.posForShadow = mul(float4(input.position, 1), shadowWVP);

	// Whatever we return will make its way through the pipeline to the
	// next programmable stage we're using (the pixel shader for now)
	return output;
}