#include "IBLCubemap.h"



IBLCubemap::IBLCubemap(ID3D11Device* device, int mipLevels, int size)
{
	D3D11_TEXTURE2D_DESC texDesc;

	texDesc.Width                = size;
	texDesc.Height               = size;
	texDesc.MipLevels            = mipLevels;
	texDesc.ArraySize            = 6; //Cubemap has 6 faces
	texDesc.SampleDesc.Count     = 1; //No multisampling
	texDesc.SampleDesc.Quality   = 0; //No multisampling
	texDesc.Format               = DXGI_FORMAT_R16G16B16A16_FLOAT; //A 4-component, 64-bit floating point format that supports 16 bits per channel, including alpha
	texDesc.Usage                = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags            = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags       = 0;
	texDesc.MiscFlags            = D3D11_RESOURCE_MISC_TEXTURECUBE;

	this->surfaces[0] = new IBLCubemapFace(device, this->texture.Get(), texDesc.Format, texDesc.Width, 0, texDesc.MipLevels);
	this->surfaces[1] = new IBLCubemapFace(device, this->texture.Get(), texDesc.Format, texDesc.Width, 1, texDesc.MipLevels);
	this->surfaces[2] = new IBLCubemapFace(device, this->texture.Get(), texDesc.Format, texDesc.Width, 2, texDesc.MipLevels);
	this->surfaces[3] = new IBLCubemapFace(device, this->texture.Get(), texDesc.Format, texDesc.Width, 3, texDesc.MipLevels);
	this->surfaces[4] = new IBLCubemapFace(device, this->texture.Get(), texDesc.Format, texDesc.Width, 4, texDesc.MipLevels);
	this->surfaces[5] = new IBLCubemapFace(device, this->texture.Get(), texDesc.Format, texDesc.Width, 5, texDesc.MipLevels);

	D3D11_SHADER_RESOURCE_VIEW_DESC texShaderResourceViewDesc;

	texShaderResourceViewDesc.Format = texDesc.Format;
	texShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	texShaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
	texShaderResourceViewDesc.Texture2D.MipLevels = texDesc.MipLevels;
}


IBLCubemap::~IBLCubemap()
{
	delete this->surfaces;
}
