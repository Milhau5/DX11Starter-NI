#pragma once

#include <Windows.h>
#include <DirectXMath.h>

class Camera
{
public:
	Camera();
	
	void Update();

	DirectX::XMFLOAT4X4 GetMatrixP();
	DirectX::XMFLOAT4X4 GetMatrixV();

	void UpdateProjectionMatrix(unsigned int w, unsigned int h);

	void UpdateXRotation();
	void UpdateYRotation();

	~Camera();
private:
	DirectX::XMFLOAT4X4 camProjMatrix;
	DirectX::XMFLOAT4X4 camViewMatrix;

	//need these to create a "look-to" view matrix
	DirectX::XMFLOAT3 camPos;
	DirectX::XMFLOAT3 camDir;
	float rotAroundX;
	float rotAroundY;
};

