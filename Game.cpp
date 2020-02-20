#include "Game.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "SimpleShader.h"

// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>

// For the DirectX Math library
using namespace DirectX;

// --------------------------------------------------------
// Constructor
//
// DXCore (base class) constructor will set up underlying fields.
// DirectX itself, and our window, are not ready yet!
//
// hInstance - the application's OS-level handle (unique ID)
// --------------------------------------------------------
Game::Game(HINSTANCE hInstance)
	: DXCore(
		hInstance,		   // The application's handle
		"DirectX Game",	   // Text for the window's title bar
		1280,			   // Width of the window's client area
		720,			   // Height of the window's client area
		true)			   // Show extra stats (fps) in title bar?
{

#if defined(DEBUG) || defined(_DEBUG)
	// Do we want a console window?  Probably only in debug mode
	CreateConsoleWindow(500, 120, 32, 120);
	printf("Console window created successfully.  Feel free to printf() here.\n");
#endif

}

// --------------------------------------------------------
// Destructor - Clean up anything our game has created:
//  - Release all DirectX objects created here
//  - Delete any objects to prevent memory leaks
// --------------------------------------------------------
Game::~Game()
{
	for (size_t i = 0; i < entities.size(); i++)
	{
		delete entities[i];
	}
	entities.clear();

	for(size_t i = 0; i < materials.size(); i++) 
	{
		delete materials[i];
	}
	materials.clear();

	delete triangleShape;
	delete squareShape;
	delete houseShape;

	delete playerCamera;

	delete pixelShader;
	delete vertexShader;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	// Helper methods for loading shaders, creating some basic
	// geometry to draw and some simple camera matrices.
	//  - You'll be expanding and/or replacing these later
	LoadShaders();

	CreateBasicGeometry();

	playerCamera = new Camera(XMFLOAT3(0,0,-4.f), XMFLOAT3(0,0,0), (float)this->width / this->height);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

// --------------------------------------------------------
// Loads shaders from compiled shader object (.cso) files
// and also created the Input Layout that describes our 
// vertex data to the rendering pipeline. 
// - Input Layout creation is done here because it must 
//    be verified against vertex shader byte code
// - We'll have that byte code already loaded below
// --------------------------------------------------------
void Game::LoadShaders()
{
	vertexShader = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"VertexShader.cso").c_str());
	pixelShader = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"PixelShader.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	
	
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.2f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(+0.2f, -0.2f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.2f, -0.2f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
	};

	Vertex vertices2[] =
	{
		{ XMFLOAT3(-0.6f, 0.6f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.4f, 0.6f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.4f, -0.6f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(-0.6f, -0.6f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) }
	};

	Vertex vertices3[] =
	{
		{ XMFLOAT3(0.6f, 0.8f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(0.85f, 0.5f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(0.35f, 0.5f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(0.85f, 0.f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(0.6f, 0.f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) },
		{ XMFLOAT3(0.35f, 0.f, +0.0f), XMFLOAT3(0,0,-1), XMFLOAT2(0,0) }
	};
	
	
	unsigned int indices[] = { 0, 1, 2 };
	unsigned int indices2[] = { 0, 1, 2, 0, 2, 3 };
	unsigned int indices3[] = { 0, 1, 2, 1, 3, 4, 2, 1, 4, 2, 4, 5 };
	
	triangleShape = new Mesh(vertices, 3, indices, 3, device.Get());
	squareShape = new Mesh(vertices2, 4, indices2, 6, device.Get());
	houseShape = new Mesh(vertices3, 6, indices3, 12, device.Get());

	// setup materials
	materials.push_back(new Material(XMFLOAT4(0, 0, 1.f, 1), vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(0, 1.f, 0, 1), vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(1, 0, 0, 1), vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(.15f, .1f, .5f, 1), vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(0.2f, 0.8f, 0, 1), vertexShader, pixelShader));

	entities.push_back(new Entity(triangleShape, materials[0]));
	entities.push_back(new Entity(triangleShape, materials[1]));
	entities.push_back(new Entity(squareShape, materials[2]));
	entities.push_back(new Entity(squareShape,  materials[3]));
	entities.push_back(new Entity(houseShape,  materials[4]));
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
	if(!playerCamera)
		return;
	playerCamera->UpdateProjectionMatrix((float)this->width / this->height);
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();

	playerCamera->Update(deltaTime, hWnd);

	if(entities.size() == 0) 
	{
		return;
	}

	float sinTime = (float)sin(totalTime);
	float offset = (sinTime*deltaTime);
	entities[0]->GetTransform()->MoveAbsolute(-offset/3.f, offset/5.f, 0);
	entities[0]->GetTransform()->SetPosition(
		entities[0]->GetTransform()->GetPosition().x,
		entities[0]->GetTransform()->GetPosition().y, -.01f
	);

	entities[1]->GetTransform()->MoveAbsolute(0, offset, 0);

	entities[2]->GetTransform()->SetPosition(-1.f*deltaTime, 0, -1.f);
	entities[2]->GetTransform()->Rotate(0, 0, 0.1f * deltaTime);

	entities[3]->GetTransform()->SetScale(1, 2.5f*deltaTime, 1);
	entities[3]->GetTransform()->MoveAbsolute(0,0, offset*2.f);
	entities[3]->GetTransform()->MoveAbsolute(offset/2.f, -offset/2.f, 0);

	entities[4]->GetTransform()->Rotate(0, 0,  offset*2.f);
	entities[4]->GetTransform()->SetPosition(0, 0, -0.05f);
}

// --------------------------------------------------------
// Clear the screen, redraw everything, present to the user
// --------------------------------------------------------
void Game::Draw(float deltaTime, float totalTime)
{
	// Background color (Cornflower Blue in this case) for clearing
	const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

	// Clear the render target and depth buffer (erases what's on the screen)
	//  - Do this ONCE PER FRAME
	//  - At the beginning of Draw (before drawing *anything*)
	context->ClearRenderTargetView(backBufferRTV.Get(), color);
	context->ClearDepthStencilView(
		depthStencilView.Get(),
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1.0f,
		0);

	for (Entity* entity : entities)
	{
		entity->Draw(context.Get(), playerCamera);
	}

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}