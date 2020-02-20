#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h>
#include <vector>

class Mesh;
class Entity;
class Camera;
class Material;
class SimplePixelShader;
class SimpleVertexShader;

class Game 
	: public DXCore
{

public:
	Game(HINSTANCE hInstance);
	~Game();

	// Overridden setup and game loop methods
	void Init();
	void OnResize();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);

private:

	// Initialization helper methods
	void LoadShaders(); 
	void CreateBasicGeometry();

	// Shaders and shader-related constructs
	class SimplePixelShader* pixelShader = nullptr;
	class SimpleVertexShader* vertexShader = nullptr;

	std::vector<Entity*> entities;

	std::vector<Material*> materials;

	Camera* playerCamera = nullptr;

	Mesh* triangleShape = nullptr;
	Mesh* squareShape = nullptr;
	Mesh* houseShape = nullptr;
};

