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

	~Camera();
private:
	DirectX::XMFLOAT4X4 camProjMatrix;
	DirectX::XMFLOAT4X4 camViewMatrix;

	//need these to create a "look-to" view matrix
	DirectX::XMFLOAT3 camPos;
	DirectX::XMFLOAT3 camDir;
	float rotAroundX; //used to be regular floats
	float rotAroundY;

	//TO-DO:
	//1. Fix keyboard input (code is done)
	//2. Fix Mouse Input (code is done)
	//3. Copy-paste code to update Projection Matrix
	//4. Materials
	//3+4 can be done on the same day, both are short
};

