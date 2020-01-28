#pragma once

#include "DXCore.h"
#include <DirectXMath.h>
#include <wrl/client.h>

class Mesh;

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
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS;

	Mesh* triangleShape = nullptr;
	Mesh* squareShape = nullptr;
	Mesh* houseShape = nullptr;
};

