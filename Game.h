#pragma once

#include "DXCore.h"
#include <wrl/client.h>
#include <vector>
#include "Lights.h"

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

	class SimplePixelShader* normalPS = nullptr;
	class SimpleVertexShader* normalVS = nullptr;

	//texture stuff
	ID3D11ShaderResourceView* srvBrick;
	ID3D11ShaderResourceView* srvMetal;
	ID3D11ShaderResourceView* srvRock;
	ID3D11ShaderResourceView* srvRockNormal;
	ID3D11ShaderResourceView* srvCushion;
	ID3D11ShaderResourceView* srvCushionNormal;
	ID3D11SamplerState* textureSampler;

	std::vector<class Entity*> entities;
	std::vector<class Material*> materials;
	std::vector<class Mesh*> meshes;

	struct Light dirLight;
	struct Light pointLight;
	struct Light dirLight3;

	class Camera* playerCamera = nullptr;

protected:
	virtual void BeginPlay();
};

