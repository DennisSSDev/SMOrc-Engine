#include "Material.h"

Material::Material(DirectX::XMFLOAT4 colorTint, ID3D11VertexShader* VS, ID3D11PixelShader* PS)
{
	this->colorTint = colorTint;
	vertShader = VS;
	pixelShader = PS;
}
