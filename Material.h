#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

class Material 
{
public:
	Material(DirectX::XMFLOAT4 colorTint, ID3D11VertexShader* VS, ID3D11PixelShader* PS);
	~Material() = default;

	inline ID3D11VertexShader* GetVertexShader() { return vertShader; }
	inline ID3D11PixelShader* GetPixelShader() { return pixelShader; }
	inline DirectX::XMFLOAT4 GetColorTint() { return colorTint; }

	inline void SetColorTint(DirectX::XMFLOAT4 tint) { colorTint = tint; }

private:
	DirectX::XMFLOAT4 colorTint;

	ID3D11VertexShader* vertShader = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;
};