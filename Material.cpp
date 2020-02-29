#include "Material.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"

Material::Material(DirectX::XMFLOAT4 colorTint, float shininess, class SimpleVertexShader* VS, class SimplePixelShader* PS)
{
	this->colorTint = colorTint;
	this->shininess = shininess;
	this->vertShader = VS;
	this->pixelShader = PS;
}

Material::Material(DirectX::XMFLOAT4 colorTint, float shininess, ID3D11ShaderResourceView* texResource, ID3D11SamplerState* sampler, SimpleVertexShader* VS, SimplePixelShader* PS)
{
	this->colorTint = colorTint;
	this->shininess = shininess;
	this->srvResource = texResource;
	textureSampler = sampler;
	this->vertShader = VS;
	this->pixelShader = PS;
}
