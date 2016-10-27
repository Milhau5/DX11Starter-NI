#include "Material.h"



Material::Material(SimpleVertexShader* v, SimplePixelShader* p, ID3D11ShaderResourceView* vw, ID3D11SamplerState* sm)
{
	vertexShader = v;
	pixelShader = p;
	view = vw;
	sample = sm;
}

SimpleVertexShader* Material::GetVertexShader()
{
	return vertexShader;
}

SimplePixelShader* Material::GetPixelShader()
{
	return pixelShader;
}

ID3D11ShaderResourceView* Material::GetShaderResourceView()
{
	return view;
}

ID3D11SamplerState* Material::GetSamplerState()
{
	return sample;
}

Material::~Material()
{
}
