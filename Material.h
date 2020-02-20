#pragma once

#include <DirectXMath.h>
#include <d3d11.h>

class SimpleVertexShader;
class SimplePixelShader;

class Material 
{
public:
	Material(DirectX::XMFLOAT4 colorTint, class SimpleVertexShader* VS, class SimplePixelShader* PS);
	~Material() = default;

	inline class SimpleVertexShader* GetVertexShader() { return vertShader; }
	inline class SimplePixelShader* GetPixelShader() { return pixelShader; }
	inline DirectX::XMFLOAT4 GetColorTint() { return colorTint; }

	inline void SetColorTint(DirectX::XMFLOAT4 tint) { colorTint = tint; }

private:

	DirectX::XMFLOAT4 colorTint;

	class SimpleVertexShader* vertShader = nullptr;
	class SimplePixelShader* pixelShader = nullptr;
};