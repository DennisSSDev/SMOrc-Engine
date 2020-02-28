#include "Material.h"
#include "SimpleShader.h"

Material::Material(DirectX::XMFLOAT4 colorTint, float shininess, class SimpleVertexShader* VS, class SimplePixelShader* PS)
{
	this->colorTint = colorTint;
	this->shininess = shininess;
	vertShader = VS;
	pixelShader = PS;
}
