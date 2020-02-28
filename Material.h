#pragma once

#include <DirectXMath.h>

class SimpleVertexShader;
class SimplePixelShader;

class Material 
{
public:
	Material(DirectX::XMFLOAT4 colorTint, float shininess, class SimpleVertexShader* VS, class SimplePixelShader* PS);
	~Material() = default;

	inline class SimpleVertexShader* GetVertexShader() { return vertShader; }
	inline class SimplePixelShader* GetPixelShader() { return pixelShader; }

	inline DirectX::XMFLOAT4 GetColorTint() { return colorTint; }
	inline float GetShininess() { return shininess; }

	inline void SetColorTint(DirectX::XMFLOAT4 tint) { colorTint = tint; }
	inline void SetShininess(float value) { shininess = shininess; }

private:

	// @todo: everything will be shiny by default. 
	// Maybe make a separate shader for none shiny objects
	// @todo make sure to clamp the value of the shininess

	DirectX::XMFLOAT4 colorTint;
	float shininess;

	class SimpleVertexShader* vertShader = nullptr;
	class SimplePixelShader* pixelShader = nullptr;
};