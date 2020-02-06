#pragma once

#include <d3d11.h>
#include "Transform.h"

class Mesh;

class Entity 
{
public:
	Entity(Mesh* incomingMesh);
	~Entity() = default;

	Mesh* GetMesh() const;
	Transform* GetTransform();

	void Draw(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer);

private:
	Transform transform;
	Mesh* mesh;
};