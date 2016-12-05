#include "Entity.h"

using namespace DirectX;

Entity::Entity(Mesh * mesh, Material * material) //will eventually take a material
{
	//accept Mesh pointer
	meshingAround = mesh;
	girlInAMaterialWorld = material;
	
	//set default values for the XMFLOATs
	worldMatrix = XMFLOAT4X4(); //just call the default constructor
	posVector = XMFLOAT3(0, 0, 0);
	rotVector = XMFLOAT3(0, 0, 0);
	scaleVector = XMFLOAT3(1, 1, 1);
}

XMFLOAT4X4 Entity::GetMatrix()
{
	return worldMatrix;
}

void Entity::SetMatrix(XMFLOAT4X4 m)
{
	worldMatrix = m;
}

XMFLOAT3 Entity::GetPosition()
{
	return posVector;
}

void Entity::SetPosition(XMFLOAT3 p)
{
	posVector = p;
}

XMFLOAT3 Entity::GetRotation()
{
	return rotVector;
}

void Entity::SetRotation(XMFLOAT3 r)
{
	rotVector = r;
}

XMFLOAT3 Entity::GetScale()
{
	return scaleVector;
}

void Entity::SetScale(XMFLOAT3 s)
{
	scaleVector = s;
}

void Entity::Move()
{
	//three temp matrices
	//edit some stuff with relevant vectors
	//multiply all three
	//XMStoreFloat4X4

	XMMATRIX transMat = XMMatrixTranslation(posVector.x, posVector.y, posVector.z);
	XMMATRIX rotMat = XMMatrixRotationZ(rotVector.z);
	XMMATRIX scaleMat = XMMatrixScaling(scaleVector.x, scaleVector.y, scaleVector.z);

	XMMATRIX zaWarudo = scaleMat * rotMat * transMat;

	XMStoreFloat4x4(
		&worldMatrix,
		XMMatrixTranspose(zaWarudo));
}

void Entity::PrepareMaterial(XMFLOAT4X4 viewMatrix, XMFLOAT4X4 projMatrix, XMFLOAT4X4 shadowView, XMFLOAT4X4 shadowProj)
{
	SimpleVertexShader* v = girlInAMaterialWorld->GetVertexShader();
	SimplePixelShader* p = girlInAMaterialWorld->GetPixelShader();
	
	// Send data to shader variables
	//  - Do this ONCE PER OBJECT you're drawing
	//  - This is actually a complex process of copying data to a local buffer
	//    and then copying that entire buffer to the GPU.  
	//  - The "SimpleShader" class handles all of that for you.
	v->SetMatrix4x4("world", GetMatrix());
	v->SetMatrix4x4("view", viewMatrix); //NOW camera's view matrix
	v->SetMatrix4x4("projection", projMatrix);
	
	v->SetMatrix4x4("shadowView", shadowView);
	v->SetMatrix4x4("shadowProjection", shadowProj);

	// Once you've set all of the data you care to change for
	// the next draw call, you need to actually send it to the GPU
	//  - If you skip this, the "SetMatrix" calls above won't make it to the GPU!
	v->CopyAllBufferData();

	v->SetShader();
	p->SetShader();
}

void Entity::Draw(ID3D11DeviceContext *context) //may take camera matrices in later versions...
{
	//jus do sum drawing sheeit
	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer * temp = meshingAround->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &temp, &stride, &offset);
	context->IASetIndexBuffer(meshingAround->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);

	// Finally do the actual drawing
	//  - Do this ONCE PER OBJECT you intend to draw
	//  - This will use all of the currently set DirectX "stuff" (shaders, buffers, etc)
	//  - DrawIndexed() uses the currently set INDEX BUFFER to look up corresponding
	//     vertices in the currently set VERTEX BUFFER
	context->DrawIndexed(
		meshingAround->GetIndexCount(),     // The number of indices to use (we could draw a subset if we wanted)
		0,     // Offset to the first index we want to use
		0);    // Offset to add to each index when looking up vertices
}

//Shadow will actually be added in Game.cpp
//we just need some slight restructuring here
void Entity::DrawWithShadow(ID3D11DeviceContext *context)
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	ID3D11Buffer * temp = meshingAround->GetVertexBuffer();
	context->IASetVertexBuffers(0, 1, &temp, &stride, &offset);
	context->IASetIndexBuffer(meshingAround->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
}

Mesh * Entity::GetMesh()
{
	return meshingAround;
}

Entity::~Entity()
{
}
