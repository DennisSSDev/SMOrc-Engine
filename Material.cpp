#include "Material.h"
#include "SimpleShader.h"

Material::Material(DirectX::XMFLOAT4 colorTint, class SimpleVertexShader* VS, class SimplePixelShader* PS)
{
	this->colorTint = colorTint;
	vertShader = VS;
	pixelShader = PS;
}
