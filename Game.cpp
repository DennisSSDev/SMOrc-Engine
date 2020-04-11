#include "Game.h"
#include "Vertex.h"
#include "Mesh.h"
#include "Entity.h"
#include "Camera.h"
#include "Material.h"
#include "SimpleShader.h"
#include "WICTextureLoader.h"
#include <ppl.h>

using namespace Concurrency;

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
	parallel_for
	(
		size_t(0), entities.size(), [&](size_t i)
		{
			delete entities[i];
		},
		static_partitioner()
	);

	parallel_for 
	(
		size_t(0), materials.size(), [&](size_t i)
		{
			delete materials[i];
		},
		static_partitioner()
	);

	parallel_for 
	(
		size_t(0), meshes.size(), [&](size_t i) 
		{
			delete meshes[i];
		}, 
		static_partitioner()
	);

	srvBrick->Release();
	srvMetal->Release();
	srvRock->Release();
	srvRockNormal->Release();
	textureSampler->Release();
	srvCushion->Release();
	srvCushionNormal->Release();

	srvBlueprintBlue->Release();
	srvBlueprintGray->Release();
	srvBlueprintOrange->Release();
	srvBlueprintDefault->Release();
	srvBlueprintGreen->Release();

	delete playerCamera;
	delete pixelShader;
	delete vertexShader;

	delete normalVS;
	delete normalPS;

	delete[] lights;
}

// --------------------------------------------------------
// Called once per program, after DirectX and the window
// are initialized but before the game loop.
// --------------------------------------------------------
void Game::Init()
{
	LoadShaders();

	CreateBasicGeometry();

	lights = new Light[MAX_LIGHTS_IN_SCENE];

	playerCamera = new Camera(XMFLOAT3(0,0,-4.f), XMFLOAT3(0,0,0), (float)this->width / this->height);

	// Tell the input assembler stage of the pipeline what kind of
	// geometric primitives (points, lines or triangles) we want to draw.  
	// Essentially: "What kind of shape should the GPU draw with our data?"
	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/**
	 * Scene Light definitions
	 */

	lights[lightsInScene].color = XMFLOAT3(1.0f, 0.2f, 0.2f);
	lights[lightsInScene].type = LIGHT_TYPE_SPOT;
	lights[lightsInScene].direction = XMFLOAT3(0.f, 1.f, 0.f);
	lights[lightsInScene].range = 4.f;
	lights[lightsInScene].intensity = 1.f;
	lights[lightsInScene].spotFalloff = 25.f;
	lights[lightsInScene++].position = XMFLOAT3(-4.5f, 2.5f, -34.5f);

	lights[lightsInScene].color = XMFLOAT3(.65f, .2f, .3f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 5.f;
	lights[lightsInScene].intensity = 2.f;
	lights[lightsInScene++].position = XMFLOAT3(0, 0, 0);

	lights[lightsInScene].color = XMFLOAT3(1.f, 1.f, 1.f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 4.f;
	lights[lightsInScene].intensity = 2.f;
	lights[lightsInScene++].position = XMFLOAT3(-7.5f, 3.f, 7.5f);

	lights[lightsInScene].color = XMFLOAT3(1.f, 1.f, 1.f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 2.5f;
	lights[lightsInScene].intensity = 2.f;
	lights[lightsInScene++].position = XMFLOAT3(-5.f, 1.85f, 1.f);

	lights[lightsInScene].color = XMFLOAT3(1.f, 1.f, 1.f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 3.f;
	lights[lightsInScene].intensity = 1.5f;
	lights[lightsInScene++].position = XMFLOAT3(-5.f, 1.85f, -11.f);

	lights[lightsInScene].color = XMFLOAT3(1.f, 1.f, 1.f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 4.5f;
	lights[lightsInScene].intensity = 1.f;
	lights[lightsInScene++].position = XMFLOAT3(5.f, 2.5f, -20.f);

	lights[lightsInScene].color = XMFLOAT3(1.f, 1.f, 1.f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 4.f;
	lights[lightsInScene].intensity = 1.f;
	lights[lightsInScene++].position = XMFLOAT3(5.f, 2.5f, -33.f);

	lights[lightsInScene].color = XMFLOAT3(.5f, 1.f, .9f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 4.f;
	lights[lightsInScene].intensity = 1.f;
	lights[lightsInScene++].position = XMFLOAT3(-4.5f, 2.5f, -34.5f);

	lights[lightsInScene].color = XMFLOAT3(.98f, .85f, .85f);
	lights[lightsInScene].type = LIGHT_TYPE_POINT;
	lights[lightsInScene].range = 4.5f;
	lights[lightsInScene].intensity = 1.f;
	lights[lightsInScene++].position = XMFLOAT3(-11.5f, 2.5f, -26.5f);

	lights[lightsInScene].color = XMFLOAT3(1.f, 1.f, 1.f);
	lights[lightsInScene].type = LIGHT_TYPE_AMBIENT;
	lights[lightsInScene++].intensity = .1f;

	// all the initialization for the engine has to be done prior to this. Now the game specific stuff needs to initialize
	BeginPlay();
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

	normalVS = new SimpleVertexShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"NormalMapVS.cso").c_str());
	normalPS = new SimplePixelShader(device.Get(), context.Get(), GetFullPathTo_Wide(L"NormalMapPS.cso").c_str());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	// setup models
    meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/sphere.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/cube.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/helix.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/torus.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/cylinder.obj").c_str(), device.Get()));

	// setup game room models
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/Rooms/BeginRoom.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/Rooms/MainRoom.obj").c_str(), device.Get()));

	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/RoomAssets/Arch.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/RoomAssets/Doorway.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/RoomAssets/Prism.obj").c_str(), device.Get()));
	meshes.push_back(new Mesh(GetFullPathTo("../../Assets/Models/RoomAssets/Pipe.obj").c_str(), device.Get()));

	D3D11_SAMPLER_DESC sampDesc = {};
	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	//sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR; // Tri-linear filtering
	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sampDesc.MaxAnisotropy = 16;
	sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	HRESULT res = device->CreateSamplerState(&sampDesc, &textureSampler);

	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"../../Assets/Textures/brick.png").c_str(), nullptr, &srvBrick);
	if(res != S_OK) 
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/metal.png").c_str(), nullptr, &srvMetal);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/rock.png").c_str(), nullptr, &srvRock);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/rock_normals.png").c_str(), nullptr, &srvRockNormal);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/cushion.png").c_str(), nullptr, &srvCushion);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/cushion_normals.png").c_str(), nullptr, &srvCushionNormal);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/GridBox_Default.png").c_str(), nullptr, &srvBlueprintDefault);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/prototype_512x512_orange.png").c_str(), nullptr, &srvBlueprintOrange);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/prototype_512x512_blue2.png").c_str(), nullptr, &srvBlueprintBlue);
	if(res != S_OK) 
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/prototype_512x512_grey2.png").c_str(), nullptr, &srvBlueprintGray);
	if (res != S_OK)
	{
		assert(false);
	}
	res = CreateWICTextureFromFile(device.Get(), context.Get(), GetFullPathTo_Wide(L"/../../Assets/Textures/prototype_512x512_green1.png").c_str(), nullptr, &srvBlueprintGreen);
	if (res != S_OK)
	{
		assert(false);
	}

	// setup materials
	// sphere gets shininess
	// uses normal maps
	materials.push_back(new Material(XMFLOAT4(1.f, 1.f, 1.f, 1.f), 5.f, srvCushion, srvCushionNormal, textureSampler, normalVS, normalPS));
	// cube gets full shininess
	materials.push_back(new Material(XMFLOAT4(.8f, .86f, .8f, 1), 1.f, srvBrick, textureSampler, vertexShader, pixelShader));
	// helix slightly less shiny
	materials.push_back(new Material(XMFLOAT4(.88f, 0.1f, .68f, 1), .75f, srvMetal, textureSampler, vertexShader, pixelShader));
	// torus barely shiny
	materials.push_back(new Material(XMFLOAT4(.75f, .75f, .8f, 1), .45f, srvRock, srvRockNormal, textureSampler, normalVS, normalPS));
	// cylinder is not going to have any shininess
	materials.push_back(new Material(XMFLOAT4(0.2f, 0.8f, .28f, 1), 0, srvMetal, textureSampler, vertexShader, pixelShader));

	/*
	Stealth Game materials go here
	*/
	materials.push_back(new Material(XMFLOAT4(1.f, 1.f, 1.f, 1.f), 0.f, srvBlueprintBlue, textureSampler, vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(1.f, 1.f, 1.f, 1.f), 0.f, srvBlueprintGray, textureSampler, vertexShader, pixelShader));

	materials.push_back(new Material(XMFLOAT4(1.f, 1.f, 1.f, 1.f), 0.f, srvBlueprintDefault, textureSampler, vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(1.f, 1.f, 1.f, 1.f), 0.f, srvBlueprintOrange, textureSampler, vertexShader, pixelShader));
	materials.push_back(new Material(XMFLOAT4(1.f, 1.f, 1.f, 1.f), 0.f, srvBlueprintGreen, textureSampler, vertexShader, pixelShader));

	// setup entities
	entities.push_back(new Entity(meshes[0], materials[0]));
	entities.push_back(new Entity(meshes[1], materials[1]));
	entities.push_back(new Entity(meshes[2], materials[2]));
	entities.push_back(new Entity(meshes[3],  materials[3]));
	entities.push_back(new Entity(meshes[4],  materials[4]));

	/*entities for the stealth game*/

	// blue building
	entities.push_back(new Entity(meshes[5], materials[5]));
	// gray building
	entities.push_back(new Entity(meshes[6], materials[6]));

	// room assets
	
	//arch
	entities.push_back(new Entity(meshes[7], materials[7]));
	//doorway
	entities.push_back(new Entity(meshes[8], materials[8]));
	//prism
	entities.push_back(new Entity(meshes[9], materials[8]));
	//pipe
	entities.push_back(new Entity(meshes[10], materials[9]));
}


void Game::BeginPlay()
{
	if(entities.size() <= 0)
		return;

	entities[0]->GetTransform()->MoveAbsolute(3, 0, 1);

	entities[1]->GetTransform()->SetPosition(.2f, 1, .5f);

	//helix
	entities[2]->GetTransform()->SetPosition(-1.5, 0, -1);
	entities[2]->GetTransform()->SetScale(.5f, .5f, .5f);

	entities[4]->GetTransform()->SetPosition(1, -1.5, -.05f);

	// stealth game related begin play
	entities[6]->GetTransform()->MoveAbsolute(0, 0, -10);

	entities[7]->GetTransform()->SetRotation(0, 35.5f, 0.f);
	entities[7]->GetTransform()->MoveAbsolute(0,0,-24);

	entities[8]->GetTransform()->MoveAbsolute(8,.5f,-27);

	entities[9]->GetTransform()->MoveAbsolute(-9,.5f,-22);

	entities[10]->GetTransform()->MoveAbsolute(-6,.5f,-34);

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

	// Handle input
	inputSystem->Frame(deltaTime, playerCamera);

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

	entities[2]->GetTransform()->Rotate(0,  1.f * deltaTime, 0);
	
	entities[3]->GetTransform()->MoveAbsolute(0,0, offset*2.f);
	entities[3]->GetTransform()->MoveAbsolute(offset/2.f, -offset/2.f, 0);
	entities[3]->GetTransform()->Rotate(-1.5f * deltaTime, 0, 0);

	entities[4]->GetTransform()->Rotate(0, 0,  offset*2.f);

	playerCamera->UpdateViewMatrix();
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

	// since they are all shared we don't need to individually set it per entity
	normalPS->SetData("lights", (void*)(lights), sizeof(Light) * lightsInScene);
	normalPS->SetInt("lightCount", lightsInScene);
	normalPS->SetFloat3("cameraPosition", playerCamera->GetTransform()->GetPosition());
	normalPS->CopyAllBufferData();

	pixelShader->SetData("lights", (void*)(lights), sizeof(Light) * lightsInScene);
	pixelShader->SetInt("lightCount", lightsInScene);
	pixelShader->SetFloat3("cameraPosition", playerCamera->GetTransform()->GetPosition());
	pixelShader->CopyAllBufferData();

	for (Entity* entity : entities)
	{
		// detect if light affects the material
		Material* entityMat = entity->GetMaterial();
		entityMat->GetVertexShader()->SetShader();
		entityMat->GetPixelShader()->SetShader();

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