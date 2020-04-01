#pragma once

#include <DirectXMath.h>

struct Light
{
	DirectX::XMFLOAT3 color;
	float intensity;
	DirectX::XMFLOAT3 direction;
	float radius;
	DirectX::XMFLOAT3 position;
	UINT type;
	float spotFalloff;
	DirectX::XMFLOAT3 pad;
};