#pragma once

#include "DXCore.h"
#include "Vertex.h"
#include "Mesh.h"
#include <d3d11.h>
#include <DirectXMath.h>

class Entity
{
public:
	Entity(Mesh * mesh);
	
	DirectX::XMFLOAT4X4 GetMatrix();
	void SetMatrix(DirectX::XMFLOAT4X4 m);

	DirectX::XMFLOAT3 GetPosition();
	void SetPosition(DirectX::XMFLOAT3 p);
	DirectX::XMFLOAT3 GetRotation();
	void SetRotation(DirectX::XMFLOAT3 r);
	DirectX::XMFLOAT3 GetScale();
	void SetScale(DirectX::XMFLOAT3 s);

	void Move();
	
	void Draw(ID3D11DeviceContext *context); //this will probably be the hardest part
	
	~Entity();
private:
	DirectX::XMFLOAT4X4 worldMatrix;
	
	DirectX::XMFLOAT3 posVector;
	DirectX::XMFLOAT3 rotVector;
	DirectX::XMFLOAT3 scaleVector;

	Mesh * meshingAround;

	//ID3D11DeviceContext *ctx;
};

