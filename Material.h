#pragma once

#include <DirectXMath.h>

class SimpleVertexShader;
class SimplePixelShader;

class Material 
{
public:

	Material(DirectX::XMFLOAT4 colorTint, float shininess, class SimpleVertexShader* VS, class SimplePixelShader* PS);
	Material(DirectX::XMFLOAT4 colorTint, float shininess, struct ID3D11ShaderResourceView* texResource, struct ID3D11SamplerState* sampler,  class SimpleVertexShader* VS, class SimplePixelShader* PS);
	~Material() = default;

	inline class SimpleVertexShader* GetVertexShader() { return vertShader; }
	inline class SimplePixelShader* GetPixelShader() { return pixelShader; }

	inline DirectX::XMFLOAT4 GetColorTint() { return colorTint; }
	inline float GetShininess() { return shininess; }

	inline ID3D11ShaderResourceView* GetSrvResource() { return srvResource; }
	inline ID3D11SamplerState* GetTextureSampler() { return textureSampler; }

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

	ID3D11ShaderResourceView* srvResource = nullptr;
	ID3D11SamplerState* textureSampler = nullptr; 
};