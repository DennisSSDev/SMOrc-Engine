#include "Transform.h"

Transform::Transform()
{
	DirectX::XMStoreFloat4x4(&worldMatrix, DirectX::XMMatrixIdentity());
	localScale = DirectX::XMFLOAT3(1,1,1);
	worldPosition = DirectX::XMFLOAT3(0,0,0);
	rotation = DirectX::XMFLOAT3(0,0,0);
	quaternionRotation = DirectX::XMFLOAT4(0,0,0,1);
}

Transform::~Transform()
{

}

void Transform::SetPosition(float x, float y, float z)
{
	worldPosition = DirectX::XMFLOAT3(x, y, z);
	MarkAsDirty();
}

void Transform::SetRotation(float pitch, float yaw, float roll)
{
	rotation = DirectX::XMFLOAT3(pitch, yaw, roll);
	DirectX::XMStoreFloat4(&quaternionRotation,  DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
	MarkAsDirty();
}

void Transform::SetScale(float x, float y, float z)
{
	localScale = DirectX::XMFLOAT3(x, y, z);
	MarkAsDirty();
}

DirectX::XMFLOAT3 Transform::GetPosition() const
{
	return worldPosition;
}

DirectX::XMFLOAT3 Transform::GetPitchYawRoll() const
{
	return rotation;
}

DirectX::XMFLOAT3 Transform::GetScale() const
{
	return localScale;
}

DirectX::XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if(isDirty) 
	{
		CalculateWorldMatrix();
		isDirty = false;
	}
	return worldMatrix;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	auto SIMD_Vec1 = DirectX::XMLoadFloat3(&worldPosition);
	auto SIMD_Vec2 = DirectX::XMLoadFloat3(&DirectX::XMFLOAT3(x, y, z));
	auto SIMD_Vec3 = DirectX::XMVectorAdd(SIMD_Vec1, SIMD_Vec2);

	DirectX::XMStoreFloat3(&worldPosition, SIMD_Vec3);

	MarkAsDirty();
}

void Transform::Rotate(float pitch, float yaw, float roll)
{
	rotation.x += pitch;
	rotation.y += yaw;
	rotation.z += roll;

	MarkAsDirty();
}

void Transform::Scale(float x, float y, float z)
{
	localScale.x *= x;
	localScale.y *= y;
	localScale.z *= z;

	MarkAsDirty();
}

void Transform::CalculateWorldMatrix()
{
	//create the translation matrix
	DirectX::XMMATRIX tranM = DirectX::XMMatrixTranslation(worldPosition.x, worldPosition.y, worldPosition.z);

	// create the rotation matrix
	DirectX::XMMATRIX rotM = DirectX::XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	//create the scale matrix
	DirectX::XMMATRIX scaleM = DirectX::XMMatrixScaling(localScale.x, localScale.y, localScale.z);

	DirectX::XMMATRIX world = scaleM * rotM * tranM; // is this in simd?

	XMStoreFloat4x4(&worldMatrix, world);
}
