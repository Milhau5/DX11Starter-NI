#include "IBLCubemapFace.h"

IBLCubemapFace::IBLCubemapFace(ID3D11Device* device, ID3D11Texture2D* texture, DXGI_FORMAT format, int size, int arrayIndex, int mipCount)
{
	for (int mipIndex = 0; mipIndex < mipCount; mipIndex++)
	{
		D3D11_RENDER_TARGET_VIEW_DESC texRenderTargetViewDesc;

		//store in memory as a series of zeros until we need to define it
		ZeroMemory(&texRenderTargetViewDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));

		//set the definition
		//https://msdn.microsoft.com/en-us/library/windows/desktop/ff476201(v=vs.85).aspx
		texRenderTargetViewDesc.Format                                   = format;
		texRenderTargetViewDesc.ViewDimension                            = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		texRenderTargetViewDesc.Texture2DArray.MipSlice                  = mipIndex;
		texRenderTargetViewDesc.Texture2DArray.ArraySize                 = 1;
		texRenderTargetViewDesc.Texture2DArray.FirstArraySlice           = arrayIndex;

   }
}


IBLCubemapFace::~IBLCubemapFace()
{
	delete this->renderTargets;
}
