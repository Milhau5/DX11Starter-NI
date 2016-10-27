#pragma once

#include "DXCore.h"
#include "SimpleShader.h"
#include <DirectXMath.h>

class Material
{
public:
	Material(SimpleVertexShader* v, SimplePixelShader* p, ID3D11ShaderResourceView* vw, ID3D11SamplerState* sm);
	SimpleVertexShader* GetVertexShader();
	SimplePixelShader* GetPixelShader();
	ID3D11ShaderResourceView* GetShaderResourceView();
	ID3D11SamplerState* GetSamplerState();
	~Material();
private:
	// Wrappers for DirectX shaders to provide simplified functionality
	SimpleVertexShader* vertexShader;
	SimplePixelShader* pixelShader;

	//For use with texturing
	ID3D11ShaderResourceView* view;
	ID3D11SamplerState* sample;
};

