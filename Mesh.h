#pragma once

#include "DXCore.h"
#include "Vertex.h"
#include <d3d11.h>
#include <DirectXMath.h>

class Mesh
{
public:
	Mesh(Vertex vert [], int noVertices, unsigned int indices [], int noIndices, ID3D11Device* device);
	Mesh(char* fileToLoad, ID3D11Device* dev);

	ID3D11Buffer * GetVertexBuffer();
	ID3D11Buffer * GetIndexBuffer();

	int GetIndexCount();

	void CreateBuffer(unsigned int count, Vertex* v, UINT* i, ID3D11Device* device);

	~Mesh();

private:
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;

	int howManyIndices;
};

