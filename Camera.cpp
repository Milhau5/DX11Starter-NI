#include "Camera.h"

using namespace DirectX;

Camera::Camera()
{
	camProjMatrix = XMFLOAT4X4();
	camViewMatrix = XMFLOAT4X4();
	camPos = XMFLOAT3(0.0f, 0.0f, -5.0f);
	camDir = XMFLOAT3(0.0f, 0.0f, 1.0f);
	//
	rotAroundX = 0;
	rotAroundY = 0;
}

//load/store camPos, camDir, and both matrices
void Camera::Update()
{
	//1.) Get quaternion and default vectors
	XMVECTOR defaultForward = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR q = XMQuaternionRotationRollPitchYaw(rotAroundX, rotAroundY, 0.0f);

	//2.) Position and Direction for the View Matrix
	XMVECTOR dirEction = XMVector3Rotate(defaultForward, q); //forward
	XMStoreFloat3(&camDir, dirEction); //stores an XMVECTOR in an XMFLOAT3

	//3.) Get our view and projection matrices
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&camPos), XMLoadFloat3(&camDir), up); //up
	XMStoreFloat4x4(&camViewMatrix, XMMatrixTranspose(view));

	//cross (for left + right movement)
	XMVECTOR cross = XMVector3Cross(XMLoadFloat3(&camDir), up); //defaultForward or camDir?
	XMFLOAT3 crossProduct;
	XMStoreFloat3(&crossProduct, cross);

	//keyboard movement code is going here
	if (GetAsyncKeyState(VK_UP)) //up
	{
		//Get direction currently facing and += it
		camPos.y += 0.0015;
	}
	if (GetAsyncKeyState(VK_DOWN)) //down
	{
		//Negation of up (forward)
		camPos.y -= 0.0015;
	}
	if (GetAsyncKeyState(VK_LEFT)) //left
	{
		//new stuff, relative
		XMStoreFloat3(&camPos, XMLoadFloat3(&camPos) - XMVectorScale(cross, 0.001f));
	}
	if (GetAsyncKeyState(VK_RIGHT)) //right
	{
		//Negation of Left
		XMStoreFloat3(&camPos, XMLoadFloat3(&camPos) + XMVectorScale(cross, 0.001f));
	}
	if (GetAsyncKeyState(VK_SPACE)) //forward
	{
		XMVECTOR rotation = XMVectorSet(0.0f, 0.0f, 0.001f, 0.0f);

		XMStoreFloat3(&camPos, XMLoadFloat3(&camPos) + rotation);
	}
	if (GetAsyncKeyState(VK_LSHIFT)) //back
	{
		XMVECTOR negarotation = XMVectorSet(0.0f, 0.0f, -0.001f, 0.0f);

		XMStoreFloat3(&camPos, XMLoadFloat3(&camPos) + negarotation);
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

void Camera::UpdateProjectionMatrix(unsigned int w, unsigned int h)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//   the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		(float)w / h,		// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&camProjMatrix, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void Camera::UpdateXRotation()
{
	rotAroundX += 0.01f;
}

void Camera::UpdateYRotation()
{
	rotAroundY += 0.01f;
}

Camera::~Camera()
{
}
