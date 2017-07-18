#pragma once
#include "IBLCubemapFace.h"
#include <wrl.h>

class IBLCubemap
{
public:
	IBLCubemap(ID3D11Device* device, int mipLevels, int size);
	
	~IBLCubemap();

	//Get addres of Shader Resource View of texture
	ID3D11ShaderResourceView** GetShaderResourceViewAddress()
	{
		//
		return this->shaderResourceView.GetAddressOf();
	};

	IBLCubemapFace* surfaces[6];

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shaderResourceView;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture;
};

