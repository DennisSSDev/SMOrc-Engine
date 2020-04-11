#pragma once

#include "DXCore.h"
#include <wrl/client.h>
#include <vector>
#include "Lights.h"

#define MAX_LIGHTS_IN_SCENE 128

#define LIGHT_TYPE_DIR 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2
#define LIGHT_TYPE_AMBIENT 3

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

	class SimplePixelShader* solidColorTransparentPS = nullptr;

	/**
	 * The current active blend state used for ghosts
	 */
	ID3D11BlendState* blendState = nullptr;

	//texture stuff
	ID3D11ShaderResourceView* srvBrick;
	ID3D11ShaderResourceView* srvMetal;
	ID3D11ShaderResourceView* srvRock;
	ID3D11ShaderResourceView* srvRockNormal;
	ID3D11ShaderResourceView* srvCushion;
	ID3D11ShaderResourceView* srvCushionNormal;
	ID3D11SamplerState* textureSampler;

	/**
	 * Stealth Game Related textures go here
	 */
	ID3D11ShaderResourceView* srvBlueprintDefault;
	ID3D11ShaderResourceView* srvBlueprintOrange;
	ID3D11ShaderResourceView* srvBlueprintBlue;
	ID3D11ShaderResourceView* srvBlueprintGray;
	ID3D11ShaderResourceView* srvBlueprintGreen;

	std::vector<class Entity*> entities;
	std::vector<class Material*> materials;
	std::vector<class Mesh*> meshes;

	size_t activeRoute = 0;
	float ghostSpeedBoost = 2.5f;
	Transform* ghostTransform = nullptr;

	std::vector<class Entity*> ghosts;

	std::vector<class Entity*> route1;
	std::vector<class Entity*> route2;

	/**
	 * DEBUG items
	 */
	bool bDrawWaypoints = false;

	struct Light* lights = nullptr; // all the lights
	int lightsInScene = 0;

	class Camera* playerCamera = nullptr;

protected:
	virtual void BeginPlay();
	virtual void SortAndRenderTransparentEntities();
};

