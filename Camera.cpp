#include "Camera.h"

Camera::Camera(DirectX::XMFLOAT3 initPosition /*= DirectX::XMFLOAT3(0,0,0)*/, DirectX::XMFLOAT3 initRotation /*= DirectX::XMFLOAT3(0,0,0)*/, float aspectRatio /*= 1920.f / 1080.f*/, float fieldOfView /*= 90.f*/, float nearClip /*= 0.01f*/, float farClip /*= 10.f*/, float movementSpeed /*= 5.f*/, float mouseLookSpeed /*= 1.f */)
{
	transform.SetPosition(initPosition.x, initPosition.y, initPosition.z);
	transform.SetRotation(initRotation.x, initRotation.y, initRotation.z);

	fov = fieldOfView;
	this->nearClip = nearClip;
	this->farClip = farClip;
	this->mouseLookSpeed = mouseLookSpeed;
	this->movementSpeed = movementSpeed;

	mousePos = {};

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	 DirectX::XMStoreFloat4x4
	 (
		 &projMatrix, 
		 DirectX::XMMatrixPerspectiveFovLH(DirectX::XM_PIDIV4, aspectRatio, nearClip, 100.0f)
	 );
}

void Camera::UpdateViewMatrix()
{
	DirectX::XMVECTOR forward = DirectX::XMVectorSet(0,0,1,0);
	DirectX::XMVECTOR rotQuat = DirectX::XMQuaternionRotationRollPitchYawFromVector
	(
		DirectX::XMLoadFloat3
		(
			&transform.GetPitchYawRoll()
		)
	);

	forward = DirectX::XMVector3Rotate(forward, rotQuat);

	DirectX::XMMATRIX lookToMatrix = DirectX::XMMatrixLookToLH
	(
		DirectX::XMLoadFloat3(&transform.GetPosition()),
		forward,
		DirectX::XMVectorSet(0, 1, 0, 0)
	);

	DirectX::XMStoreFloat4x4(&viewMatrix, lookToMatrix);
}

void Camera::Update(float dt, HWND windowHandle)
{
	float speed = dt * movementSpeed;

	if(GetAsyncKeyState('W') & 0x8000) 
	{
		transform.MoveRelative(0, 0, speed);
	}
	if(GetAsyncKeyState('S') & 0x8000)
	{
		transform.MoveRelative(0, 0, -speed);
	}
	if(GetAsyncKeyState('D') & 0x8000) 
	{
		transform.MoveRelative(speed, 0, 0);
	}
	
	if (GetAsyncKeyState('A') & 0x8000)
	{
		transform.MoveRelative(-speed, 0, 0);
	}
	if(GetAsyncKeyState(VK_SPACE) & 0x8000) 
	{
		transform.MoveAbsolute(0, speed, 0);
	}
	if (GetAsyncKeyState('X') & 0x8000)
	{
		transform.MoveAbsolute(0, -speed, 0);
	}

	POINT newMousePos = {};
	GetCursorPos(&newMousePos);
	ScreenToClient(windowHandle, &newMousePos);

	if(GetAsyncKeyState(VK_LBUTTON) & 0x8000) 
	{
		float x = (float)newMousePos.x - mousePos.x;
		float y = (float)newMousePos.y - mousePos.y;
		x *= mouseLookSpeed * dt;
		y *= mouseLookSpeed * dt;
		transform.Rotate(y, x, 0.f);
	}
	mousePos = newMousePos;
	UpdateViewMatrix();
}
