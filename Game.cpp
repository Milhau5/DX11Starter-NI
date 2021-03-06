#include "Game.h"
#include "Vertex.h"
#include "WICTextureLoader.h"
#include "DDSTextureLoader.h"

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore( 
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{
	// Initialize fields
	vertexShader = 0;
	pixelShader = 0;

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.");
#endif
}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	// Release any (and all!) DirectX objects
	// we've made in the Game class

	// Delete our simple shader objects, which
	// will clean up their own internal DirectX stuff
	delete vertexShader;
	delete pixelShader;
	delete skyVS;
	delete skyPS;

	//get rid of meshes when done
	/*delete timmy;
	delete wanda;
	delete cosmo;*/

	delete timmy;

	delete one;
	delete two;

	delete camNewton;

	delete test;

	if (resource) { resource->Release(); }
	if (freeSamples) { freeSamples->Release(); }

	//sampler->Release(); //come back in
	//textureSRV->Release();
	//normalMapSRV->Release();
	skySRV->Release();
	skyDepthState->Release();
	skyRastState->Release();

	shadowDSV->Release();
	shadowSRV->Release();
	shadowRasterizer->Release();
	shadowSampler->Release();
	rsNoCull->Release();
	blendState->Release();
	delete shadowVS;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	shadowMapSize = 1024; //good default size
	
	camNewton = new Camera();

	camNewton->UpdateProjectionMatrix(width, height); //should be called here

	dLightful.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	dLightful.DiffuseColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	dLightful.Direction = XMFLOAT3(+1.0f, -1.0f, +0.0f);
	//
	secondLight.AmbientColor = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	secondLight.DiffuseColor = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	secondLight.Direction = XMFLOAT3(-1.0f, -1.0f, +0.0f);
	//
	/*spotMe.AmbientColor = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	spotMe.Position = XMFLOAT3(1.0f, 0.0f, 0.0f);
	spotMe.DiffuseIntensity = 2.0f;
	spotMe.Direction = XMFLOAT3(-1.0f, +0.0f, +0.0f);*/

	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();
	CreateMatrices();
	CreateBasicGeometry();

	//TEXTURE code here (maybe)

	// Load the cube map (without mipmaps!  Don't pass in the context)
	CreateDDSTextureFromFile(device, L"Debug/Assets/Textures/Ni.dds", 0, &skySRV);

	// Create a rasterizer state so we can render backfaces
	D3D11_RASTERIZER_DESC rsDesc = {};
	rsDesc.FillMode = D3D11_FILL_SOLID;
	rsDesc.CullMode = D3D11_CULL_FRONT;
	rsDesc.DepthClipEnable = true;
	device->CreateRasterizerState(&rsDesc, &skyRastState);

	// Create a depth state so that we can accept pixels
	// at a depth less than or EQUAL TO an existing depth
	D3D11_DEPTH_STENCIL_DESC dsDesc = {};
	dsDesc.DepthEnable = true;
	dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL; // Make sure we can see the sky (at max depth)
	device->CreateDepthStencilState(&dsDesc, &skyDepthState);

	// Set up a rasterizer state with no culling
	D3D11_RASTERIZER_DESC rd = {};
	rd.CullMode = D3D11_CULL_NONE;
	rd.FillMode = D3D11_FILL_SOLID;
	rd.DepthClipEnable = true;
	device->CreateRasterizerState(&rd, &rsNoCull);

	//create description of blend state here (move code if necessary)
	//blend so as to create transparency on desired object
	D3D11_BLEND_DESC bd = {};
	bd.AlphaToCoverageEnable = false;
	bd.IndependentBlendEnable = false;
	bd.RenderTarget[0].BlendEnable = true; //we might change it to [1] to get the second object
	bd.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA; //how to interpret source pixel color, src is NEW color. Option: mode by own alpha val
	bd.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA; //destination pixel color, already in render target. Option: mod by (1.0-Src pixel alpha)
	bd.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD; //operation: add
	bd.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	bd.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	bd.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	bd.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	device->CreateBlendState(&bd, &blendState);

	//----------What we need for the shadow----------

	//The texture that will become the shadow map
	D3D11_TEXTURE2D_DESC shadowDesc = {};
	shadowDesc.Width = shadowMapSize;
	shadowDesc.Height = shadowMapSize;
	shadowDesc.ArraySize = 1;
	shadowDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowDesc.CPUAccessFlags = 0;
	shadowDesc.Format = DXGI_FORMAT_R32_TYPELESS; //check the graphics debugger for this type of object!
	shadowDesc.MipLevels = 1;
	shadowDesc.MiscFlags = 0;
	shadowDesc.SampleDesc.Count = 1;
	shadowDesc.SampleDesc.Quality = 0;
	shadowDesc.Usage = D3D11_USAGE_DEFAULT;
	ID3D11Texture2D* shadowTexture;
	device->CreateTexture2D(&shadowDesc, 0, &shadowTexture);

	//Depth Stencil View
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowDSDesc = {};
	shadowDSDesc.Format = DXGI_FORMAT_D32_FLOAT;
	shadowDSDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	shadowDSDesc.Texture2D.MipSlice = 0;
	device->CreateDepthStencilView(shadowTexture, &shadowDSDesc, &shadowDSV);

	//SRV for the shadow map
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	srvDesc.Texture2D.MostDetailedMip = 0;
	device->CreateShaderResourceView(shadowTexture, &srvDesc, &shadowSRV);

	//Now release reference, since we don't need it
	shadowTexture->Release();

	//A special "comparison" sampler state for shadows
	//This will compare each pixel to a value to see which pixels are "lit" or "unlit"
	D3D11_SAMPLER_DESC shadowSampDesc = {};
	shadowSampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR; // use LERP for minification, magnification, and mip-level sampling
	shadowSampDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
	shadowSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSampDesc.BorderColor[0] = 1.0f;
	shadowSampDesc.BorderColor[1] = 1.0f;
	shadowSampDesc.BorderColor[2] = 1.0f;
	shadowSampDesc.BorderColor[3] = 1.0f;
	device->CreateSamplerState(&shadowSampDesc, &shadowSampler);

	//Rasterizer state for shadow
	D3D11_RASTERIZER_DESC shadowRastDesc = {};
	shadowRastDesc.FillMode = D3D11_FILL_SOLID;
	shadowRastDesc.CullMode = D3D11_CULL_BACK;
	shadowRastDesc.DepthClipEnable = true;
	shadowRastDesc.DepthBias = 1000; // Multiplied by (smallest possible value > 0 in depth buffer) to prevent "shadow acne"
	shadowRastDesc.DepthBiasClamp = 0.0f;
	shadowRastDesc.SlopeScaledDepthBias = 1.0f;
	device->CreateRasterizerState(&shadowRastDesc, &shadowRasterizer);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files using
// my SimpleShader wrapper for DirectX shader manipulation.
// - SimpleShader provides helpful methods for sending
//   data to individual variables on the GPU
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device, context);
	if (!vertexShader->LoadShaderFile(L"Debug/VertexShader.cso"))
		vertexShader->LoadShaderFile(L"VertexShader.cso");		

	pixelShader = new SimplePixelShader(device, context);
	if(!pixelShader->LoadShaderFile(L"Debug/PixelShader.cso"))	
		pixelShader->LoadShaderFile(L"PixelShader.cso");

	skyVS = new SimpleVertexShader(device, context);
	if (!skyVS->LoadShaderFile(L"Debug/SkyVS.cso"))
		skyVS->LoadShaderFile(L"SkyVS.cso");

	skyPS = new SimplePixelShader(device, context);
	if (!skyPS->LoadShaderFile(L"Debug/SkyPS.cso"))
		skyPS->LoadShaderFile(L"SkyPS.cso");

	shadowVS = new SimpleVertexShader(device, context);
	if (!shadowVS->LoadShaderFile(L"Debug/ShadowVS.cso"))
		shadowVS->LoadShaderFile(L"ShadowVS.cso");

	// You'll notice that the code above attempts to load each
	// compiled shader file (.cso) from two different relative paths.

	// This is because the "working directory" (where relative paths begin)
	// will be different during the following two scenarios:
	//  - Debugging in VS: The "Project Directory" (where your .cpp files are) 
	//  - Run .exe directly: The "Output Directory" (where the .exe & .cso files are)

	// Checking both paths is the easiest way to ensure both 
	// scenarios work correctly, although others exist
}



// --------------------------------------------------------
// Initializes the matrices necessary to represent our geometry's 
// transformations and our 3D camera
// --------------------------------------------------------
void Game::CreateMatrices()
{
	// Set up world matrix
	// - In an actual game, each object will need one of these and they should
	//   update when/if the object moves (every frame)
	// - You'll notice a "transpose" happening below, which is redundant for
	//   an identity matrix.  This is just to show that HLSL expects a different
	//   matrix (column major vs row major) than the DirectX Math library
	XMMATRIX W = XMMatrixIdentity();
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(W)); // Transpose for HLSL!

	// Create the View matrix
	// - In an actual game, recreate this matrix every time the camera 
	//    moves (potentially every frame)
	// - We're using the LOOK TO function, which takes the position of the
	//    camera and the direction vector along which to look (as well as "up")
	// - Another option is the LOOK AT function, to look towards a specific
	//    point in 3D space
	XMVECTOR pos = XMVectorSet(0, 0, -5, 0);
	XMVECTOR dir = XMVectorSet(0, 0, 1, 0);
	XMVECTOR up  = XMVectorSet(0, 1, 0, 0);
	XMMATRIX V   = XMMatrixLookToLH(
		pos,     // The position of the "camera"
		dir,     // Direction the camera is looking
		up);     // "Up" direction in 3D space (prevents roll)
	XMStoreFloat4x4(&viewMatrix, XMMatrixTranspose(V)); // Transpose for HLSL!

	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//   the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)width / height,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!

	//----------Shadow-related matrices (for directional lights)----------
	
	//for both matrices, we want to see things from the light's POV, as if
	//the light is a camera
	
	//need a "LookTo" view matrix
	XMMATRIX shView = XMMatrixLookAtLH(
		XMVectorSet(0, 20, -20, 0),
		XMVectorSet(0, 0, 0, 0),
		XMVectorSet(0, 1, 0, 0));
	XMStoreFloat4x4(&shadowViewMatrix, XMMatrixTranspose(shView));

	//Orthographic projection matrix
	//For Spot lights, we need a perspective projection matrix (could be XMMatrixPerspectiveLH()), pass in camera's info
	XMMATRIX shProj = XMMatrixOrthographicLH(10, 10, 0.1f, 100.0f);
	XMStoreFloat4x4(&shadowProjectionMatrix, XMMatrixTranspose(shProj));
}


// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red	= XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green	= XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue	= XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Set up the vertices of the triangle we would like to draw
	// - We're going to copy this array, exactly as it exists in memory
	//    over to a DirectX-controlled data structure (the vertex buffer)
	Vertex vertices[] = 
	{
		{ XMFLOAT3(+0.0f, +1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+1.5f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(-1.5f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	//Shape 2
	Vertex verticesTwo[] =
	{
		{ XMFLOAT3(+0.0f, -1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },  //original verts: opposite of 1
		{ XMFLOAT3(-1.5f, +1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+1.5f, +1.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
	};

	//Shape 3
	Vertex verticesThree[] =
	{
		{ XMFLOAT3(-2.0f, -2.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }, //original verts: all +- 1's instead of 2's
		{ XMFLOAT3(-2.0f, +2.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+2.0f, +2.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) },
		{ XMFLOAT3(+2.0f, -2.0f, +0.0f), XMFLOAT3(+0.0f, +0.0f, -1.0f), XMFLOAT2(0.0f, 0.0f) }
	};

	// Set up the indices, which tell us which vertices to use and in which order
	// - This is somewhat redundant for just 3 vertices (it's a simple example)
	// - Indices are technically not required if the vertices are in the buffer 
	//    in the correct order and each one will be used exactly once
	// - But just to see how it's done...
	unsigned int indices[] = { 0, 1, 2 }; //MUST be unsigned

	unsigned int indicesTwo[] = { 0, 1, 2 };

	unsigned int indicesThree[] = { 0, 1, 2, 0, 2, 3 };

	//
	HRESULT result = CreateWICTextureFromFile(device, context, L"Debug/Assets/Textures/eric_andre.jpg", 0, &resource);

	D3D11_SAMPLER_DESC sampleState = {};
	sampleState.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //Other options: Mirror, Clamp, Border, Mirror_Once
	sampleState.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleState.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sampleState.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; //trinlinear filtering
	sampleState.MaxLOD = D3D11_FLOAT32_MAX; //enable mipmapping

	device->CreateSamplerState(&sampleState, &freeSamples);
	//break point here to verify if it is working

	pixelShader->SetSamplerState("basicSampler", freeSamples);
	pixelShader->SetShaderResourceView("diffuseTexture", resource);

	//timmy = new Mesh(vertices, 3, indices, 3, device);
	timmy = new Mesh("Debug/Assets/Models/cube.obj", device);
	
	test = new Material(vertexShader, pixelShader, resource, freeSamples);

	//Create two more shapes. Make vertexes and indices, and then create Mesh objects with those params
	//wanda = new Mesh(verticesTwo, 3, indicesTwo, 3, device);
	//cosmo = new Mesh(verticesThree, 4, indicesThree, 6, device);

	//test entities...have several share one shape
	one = new Entity(timmy, test);
	two = new Entity(timmy, test);
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();

	// Update our projection matrix since the window size changed
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,	// Field of View Angle
		(float)width / height,	// Aspect ratio
		0.1f,				  	// Near clip plane distance
		100.0f);			  	// Far clip plane distance
	XMStoreFloat4x4(&projectionMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!

	camNewton->UpdateProjectionMatrix(width, height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	// Move the triangle a little
	float sinTime = (sin(totalTime) + 2.0f) / 10.0f;

	XMFLOAT3 posChange = XMFLOAT3(1.0f, 0.0f, 0.0f); 
	XMFLOAT3 rotChange = XMFLOAT3(0.0f, 0.0f, totalTime);
	XMFLOAT3 scaleChange = XMFLOAT3(sinTime, sinTime, sinTime);
	//
	XMFLOAT3 posChangeT = XMFLOAT3(0.0f, sinTime, 0.0f); //1.0f on the X

	one->SetPosition(posChange);
	one->SetRotation(rotChange);
	one->SetScale(scaleChange);
	//
	two->SetPosition(posChangeT);

	//Change relevant vectors
	//Then call "one->Move()"
	one->Move();
	two->Move();

	camNewton->Update();
}

// --------------------------------------------------------
// The method that will actually render the shadow map
// --------------------------------------------------------
void Game::RenderShadowMap()
{
	// Set up targets
	context->OMSetRenderTargets(0, 0, shadowDSV);
	context->ClearDepthStencilView(shadowDSV, D3D11_CLEAR_DEPTH, 1.0f, 0);
	context->RSSetState(shadowRasterizer);

	// Make a viewport to match the render target size
	D3D11_VIEWPORT viewport = {};
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)shadowMapSize;
	viewport.Height = (float)shadowMapSize;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	// Set up our shadow VS shader
	shadowVS->SetShader();
	shadowVS->SetMatrix4x4("view", shadowViewMatrix);
	shadowVS->SetMatrix4x4("projection", shadowProjectionMatrix);

	// Turn off pixel shader
	context->PSSetShader(0, 0, 0);

	// Grab the data from the first entity's mesh
	one->DrawWithShadow(context);
	shadowVS->SetMatrix4x4("world", one->GetMatrix());
	shadowVS->CopyAllBufferData();
	// Finally do the actual drawing
	context->DrawIndexed(one->GetMesh()->GetIndexCount(), 0, 0);

	// Grab the data from the second entity's mesh
	two->DrawWithShadow(context);
	shadowVS->SetMatrix4x4("world", two->GetMatrix());
	shadowVS->CopyAllBufferData();
	// Finally do the actual drawing
	context->DrawIndexed(two->GetMesh()->GetIndexCount(), 0, 0);
	

	// Change everything back
	context->OMSetRenderTargets(1, &backBufferRTV, depthStencilView);
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	context->RSSetViewports(1, &viewport);
	context->RSSetState(0);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime) //later, uncomment the shadow code
{
	//Shadows
	RenderShadowMap();
	
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = {0.4f, 0.6f, 0.75f, 0.0f};

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV, color);
	context->ClearDepthStencilView(
		depthStencilView, 
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	pixelShader->SetData(
		"light",
		&dLightful, //same as above?
		sizeof(DirectionalLight)
	);

	pixelShader->SetData(
		"newLight",
		&secondLight, //same as above?
		sizeof(DirectionalLight)
	);

	/*pixelShader->SetData(
		"spotLight",
		&spotMe, //same as above?
		sizeof(SpotLight)
	);*/

	//new
	pixelShader->SetShaderResourceView("ShadowMap", shadowSRV);
	pixelShader->SetSamplerState("ShadowSampler", shadowSampler);

	pixelShader->CopyAllBufferData();
	pixelShader->SetShader();
	
	one->PrepareMaterial(camNewton->GetMatrixV(), camNewton->GetMatrixP(), shadowViewMatrix, shadowProjectionMatrix);
	one->Draw(context);

    //
	
	two->PrepareMaterial(camNewton->GetMatrixV(), camNewton->GetMatrixP(), shadowViewMatrix, shadowProjectionMatrix);
	two->Draw(context);

	// After drawing objects - Draw the sky!

	// Grab the buffers
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer* skyVB = timmy->GetVertexBuffer();
	ID3D11Buffer* skyIB = timmy->GetIndexBuffer();
	context->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	context->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);

	// Set up shaders
	skyVS->SetMatrix4x4("view", camNewton->GetMatrixV());
	skyVS->SetMatrix4x4("projection", camNewton->GetMatrixP());
	skyVS->CopyAllBufferData();
	skyVS->SetShader();

	skyPS->SetShaderResourceView("Sky", skySRV);
	skyPS->CopyAllBufferData();
	skyPS->SetShader();

	//Shadow matrices, DO NOT NEED THIS CODE
	//vertexShader->SetMatrix4x4("shadowView", shadowViewMatrix);
	//vertexShader->SetMatrix4x4("shadowProjection", shadowProjectionMatrix);

	// Set the proper render states
	context->RSSetState(skyRastState);
	context->OMSetDepthStencilState(skyDepthState, 0);

	// Actually draw
	context->DrawIndexed(timmy->GetIndexCount(), 0, 0);

	// Reset the states! Supposedly this piece of code was missing...but here it is, in the right place
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
	pixelShader->SetShaderResourceView("ShadowMap", 0); //new

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);
}


#pragma region Mouse Input
// --------------------------------------------------------
// Helper method for mouse clicking.  We get this information
// from the OS-level messages anyway, so these helpers have
// been created to provide basic mouse input if you want it.
// --------------------------------------------------------
void Game::OnMouseDown(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	
	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;

	// Caputure the mouse so we keep getting mouse move
	// events even if the mouse leaves the window.  we'll be
	// releasing the capture once a mouse button is released
	SetCapture(hWnd);
}

// --------------------------------------------------------
// Helper method for mouse release
// --------------------------------------------------------
void Game::OnMouseUp(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...

	// We don't care about the tracking the cursor outside
	// the window anymore (we're not dragging if the mouse is up)
	ReleaseCapture();
}

// --------------------------------------------------------
// Helper method for mouse movement.  We only get this message
// if the mouse is currently over the window, or if we're 
// currently capturing the mouse.
// --------------------------------------------------------
void Game::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	if (buttonState & 0x0001) { camNewton->UpdateXRotation(); }

	if (buttonState & 0x0002) { camNewton->UpdateYRotation(); }

	// Save the previous mouse position, so we have it for the future
	prevMousePos.x = x;
	prevMousePos.y = y;
}

// --------------------------------------------------------
// Helper method for mouse wheel scrolling.  
// WheelDelta may be positive or negative, depending 
// on the direction of the scroll
// --------------------------------------------------------
void Game::OnMouseWheel(float wheelDelta, int x, int y)
{
	// Add any custom code here...
}
#pragma endregion