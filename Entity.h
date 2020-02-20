#pragma once

#include <d3d11.h>
#include "Transform.h"

class Mesh;
class Camera;
class Material;

class Entity 
{
public:
	Entity(Mesh* incomingMesh, Material* incomingMaterial);
	~Entity() = default;

	Mesh* GetMesh() const;
	Transform* GetTransform();

	void Draw(ID3D11DeviceContext* context, Camera* mainCamera);

private:
	Transform transform;
	class Mesh* mesh;
	class Material* material;
};