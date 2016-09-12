#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
	camProjMatrix = XMFLOAT4X4();
	camViewMatrix = XMFLOAT4X4();
	camPos = XMFLOAT3(0.0f, 0.0f, 0.0f);
	camDir = XMFLOAT3(0.0f, 0.0f, 0.0f);
	//
	rotAroundX = camDir.x;
	rotAroundY = camDir.y;
}

//load/store camPos, camDir, and both matrices
void Camera::Update()
{
	//1.) Get quaternion and default vectors
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR q = XMQuaternionRotationRollPitchYaw(rotAroundX, rotAroundY, 0.0f); //READ ACESS VIOLATION

	//2.) Position and Direction for the View Matrix
	XMVECTOR dirEction = XMVector3Rotate(defaultForward, q); //forward
	XMStoreFloat3(&camDir, dirEction); //stores an XMVECTOR in an XMFLOAT3
	//XMStoreFloat3(&camPos, poSition);

	//3.) Get our view and projection matrices
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&camPos), XMLoadFloat3(&camDir), up); //up
	XMStoreFloat4x4(&camViewMatrix, view);
	//XMStoreFloat4x4(&camProjMatrix, projection);

	//cross
	XMVECTOR cross = XMVector3Cross(defaultForward, up);
	float crossProduct;
	XMStoreFloat(&crossProduct, cross);

	//keyboard movement code is going here
	//later: scale movement speed by deltaTime
	if (GetAsyncKeyState(VK_UP))
	{
		//Get direction currently facing and += it
		camDir.x += 1.0;
	}
	if (GetAsyncKeyState(VK_DOWN))
	{
		//Negation of up (forward)
		camDir.x -= 1.0;
	}
	if (GetAsyncKeyState(VK_LEFT))
	{
		//Get cross product of forward and up, and += it by some amount
		crossProduct += 1.0;
	}
	if (GetAsyncKeyState(VK_RIGHT))
	{
		//Negation of Left
		crossProduct -= 1.0;
	}
	if (GetAsyncKeyState(VK_SPACE))
	{
		//camPos->y += 1.5;
		camPos.y += 1.5;
	}
	if (GetAsyncKeyState(VK_LSHIFT))
	{
		//camPos->y -= 1.5;
		camPos.y -= 1.5;
	}
}

XMFLOAT4X4 Camera::GetMatrixP()
{
	return camProjMatrix;
}

XMFLOAT4X4 Camera::GetMatrixV()
{
	return camViewMatrix;
}

Camera::~Camera()
{
}
