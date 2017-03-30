#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "Lights.h"
#include <DirectXMath.h>

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods, which
	// will be called automatically
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void RenderShadowMap();
	void Draw(float deltaTime, float totalTime);

	// Overridden mouse input helper methods
	void OnMouseDown (WPARAM buttonState, int x, int y);
	void OnMouseUp	 (WPARAM buttonState, int x, int y);
	void OnMouseMove (WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta,   int x, int y);
private:

	//THIS LINE OF CODE IS A TEST COMMIT FOR THE CLONED REPO
	
	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders(); 
	void CreateMatrices();
	void CreateBasicGeometry();

	// Texture related DX stuff, may not need some of the things
	ID3D11ShaderResourceView* textureSRV;
	ID3D11ShaderResourceView* normalMapSRV;
	ID3D11ShaderResourceView* skySRV;
	ID3D11SamplerState* sampler;

	// Render states
	ID3D11RasterizerState* skyRastState;
	ID3D11DepthStencilState* skyDepthState;

	//meshes
	Mesh * timmy;
	Mesh * wanda;
	Mesh * cosmo;

	//Entities
	Entity * one;
	Entity * two;
	Entity * three;

	//Camera stuff
	Camera * camNewton;
	DirectX::XMFLOAT4X4 holdCamMatrix;

	//Material(s)
	Material * test;

	//Light(s)
	DirectionalLight dLightful;
	DirectionalLight secondLight;
	//SpotLight spotMe;

	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	SimpleVertexShader* skyVS;
	SimplePixelShader* skyPS;

	//Things we will need for the Shadow Map
	int shadowMapSize;
	ID3D11DepthStencilView* shadowDSV;
	ID3D11ShaderResourceView* shadowSRV;
	ID3D11SamplerState* shadowSampler;
	ID3D11RasterizerState* shadowRasterizer;
	ID3D11RasterizerState* rsNoCull;
	ID3D11BlendState* blendState; //will help with transparency
	SimpleVertexShader* shadowVS;
	DirectX::XMFLOAT4X4 shadowViewMatrix;
	DirectX::XMFLOAT4X4 shadowProjectionMatrix;

	// The matrices to go from model space to screen space
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projectionMatrix;

	// Keeps track of the old mouse position.  Useful for 
	// determining how far the mouse moved in a single frame.
	POINT prevMousePos;

	ID3D11ShaderResourceView* resource; //how to initialize?
	ID3D11SamplerState* freeSamples;
};

