#pragma once

#include <Windows.h>
#include <DirectXMath.h>
#include "Transform.h"

class Camera 
{
public:
	Camera(
		DirectX::XMFLOAT3 initPosition = DirectX::XMFLOAT3(0,0,0), 
		DirectX::XMFLOAT3 initRotation = DirectX::XMFLOAT3(0,0,0), 
		float aspectRatio = 1920.f / 1080.f,
		float fieldOfView = 90.f, 
		float nearClip = 0.01f, 
		float farClip = 900.f,
		float movementSpeed = 5.f,
		float mouseLookSpeed = 2.f
	);
	~Camera() = default;

	void UpdateProjectionMatrix(float aspectRatio);
	void UpdateViewMatrix();
	void Update(float dt, HWND windowHandle);

	inline DirectX::XMFLOAT4X4 GetProjectionMatrix() { return projMatrix; };
	inline DirectX::XMFLOAT4X4 GetViewMatrix() { return viewMatrix; };
	inline Transform* GetTransform() { return &transform; }
private:
	Transform transform;
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;
	
	POINT mousePos;

	float fov;
	float nearClip;
	float farClip;

	float movementSpeed;
	float mouseLookSpeed;
};
