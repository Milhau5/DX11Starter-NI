#include "Entity.h"

using namespace DirectX;

Entity::Entity(Mesh * mesh)
{
	//accept Mesh pointer
	meshingAround = mesh;
	
	//set default values for the XMFLOATs
	worldMatrix = XMFLOAT4X4(); //just call the default constructor
	posVector = XMFLOAT3(0, 0, 0);
	rotVector = XMFLOAT3(0, 0, 0); //mesh rotates around the z-axis, usually
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

void Entity::Draw(ID3D11DeviceContext *context)
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

Entity::~Entity()
{
}
