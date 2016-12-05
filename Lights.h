#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct DirectionalLight
{
	DirectX::XMFLOAT4 AmbientColor;
	DirectX::XMFLOAT4 DiffuseColor;
	DirectX::XMFLOAT3 Direction;
	//float nope;
};

/*struct SpotLight //?
{
	DirectX::XMFLOAT4 AmbientColor;
	DirectX::XMFLOAT3 Position;
	float DiffuseIntensity;
	DirectX::XMFLOAT3 Direction;
	float nope;
};*/
