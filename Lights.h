#pragma once

#include <DirectXMath.h>

struct Light
{
	DirectX::XMFLOAT3 ambientColor;
	float ambientIntensity;
	DirectX::XMFLOAT3 diffuseColor;
	float diffuseIntensity;
	DirectX::XMFLOAT3 direction;
	float padding2;
	DirectX::XMFLOAT3 position;
	UINT type;
};