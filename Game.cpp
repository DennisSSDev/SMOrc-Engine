#include "Game.h"
#include "Vertex.h"
#include "Mesh.h"
#include "BufferStructs.h"

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
	delete triangleShape;
	delete squareShape;
	delete houseShape;
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

	if(device) 
	{
		unsigned int size = sizeof(VertexShaderExternalData); 
		size = (size + 15) / 16 * 16;
		
		// Describe the constant buffer
		D3D11_BUFFER_DESC cbDesc = {}; 
		
		// Sets struct to all zeros 
		cbDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; 
		cbDesc.ByteWidth = size; 

		// Must be a multiple of 16
		cbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 
		cbDesc.Usage = D3D11_USAGE_DYNAMIC; 

		device->CreateBuffer(&cbDesc, 0, constantBufferVS.GetAddressOf());
	}

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
	// Blob for reading raw data
	// - This is a simplified way of handling raw data
	ID3DBlob* shaderBlob;

	// Read our compiled vertex shader code into a blob
	// - Essentially just "open the file and plop its contents here"
	D3DReadFileToBlob(
		GetFullPathTo_Wide(L"VertexShader.cso").c_str(), // Using a custom helper for file paths
		&shaderBlob);

	// Create a vertex shader from the information we
	// have read into the blob above
	// - A blob can give a pointer to its contents, and knows its own size
	device->CreateVertexShader(
		shaderBlob->GetBufferPointer(), // Get a pointer to the blob's contents
		shaderBlob->GetBufferSize(),	// How big is that data?
		0,								// No classes in this shader
		vertexShader.GetAddressOf());	// The address of the ID3D11VertexShader*


	// Create an input layout that describes the vertex format
	// used by the vertex shader we're using
	//  - This is used by the pipeline to know how to interpret the raw data
	//     sitting inside a vertex buffer
	//  - Doing this NOW because it requires a vertex shader's byte code to verify against!
	//  - Luckily, we already have that loaded (the blob above)
	D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

	// Set up the first element - a position, which is 3 float values
	inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
	inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
	inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

	// Set up the second element - a color, which is 4 more float values
	inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
	inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
	inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

	// Create the input layout, verifying our description against actual shader code
	device->CreateInputLayout(
		inputElements,					// An array of descriptions
		2,								// How many elements in that array
		shaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
		shaderBlob->GetBufferSize(),	// Size of the shader code that uses this layout
		inputLayout.GetAddressOf());	// Address of the resulting ID3D11InputLayout*



	// Read and create the pixel shader
	//  - Reusing the same blob here, since we're done with the vert shader code
	D3DReadFileToBlob(
		GetFullPathTo_Wide(L"PixelShader.cso").c_str(), // Using a custom helper for file paths
		&shaderBlob);

	device->CreatePixelShader(
		shaderBlob->GetBufferPointer(),
		shaderBlob->GetBufferSize(),
		0,
		pixelShader.GetAddressOf());
}



// --------------------------------------------------------
// Creates the geometry we're going to draw - a single triangle for now
// --------------------------------------------------------
void Game::CreateBasicGeometry()
{
	
	// Create some temporary variables to represent colors
	// - Not necessary, just makes things more readable
	XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

	// Unity Shader Error color
	XMFLOAT4 pink = XMFLOAT4(1.0f, 0.f, 0.4f, 1.0f);

	// XBA Shader Error color
	XMFLOAT4 brightBlue = XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f);
	
	
	Vertex vertices[] =
	{
		{ XMFLOAT3(+0.0f, +0.2f, +0.0f), red },
		{ XMFLOAT3(+0.2f, -0.2f, +0.0f), blue },
		{ XMFLOAT3(-0.2f, -0.2f, +0.0f), green },
	};

	Vertex vertices2[] =
	{
		{ XMFLOAT3(-0.6f, 0.6f, +0.0f), pink },
		{ XMFLOAT3(-0.4f, 0.6f, +0.0f), blue },
		{ XMFLOAT3(-0.4f, -0.6f, +0.0f), brightBlue },
		{ XMFLOAT3(-0.6f, -0.6f, +0.0f), pink }
	};

	Vertex vertices3[] =
	{
		{ XMFLOAT3(0.6f, 0.8f, +0.0f), pink },
		{ XMFLOAT3(0.85f, 0.5f, +0.0f), blue },
		{ XMFLOAT3(0.35f, 0.5f, +0.0f), brightBlue },
		{ XMFLOAT3(0.85f, 0.f, +0.0f), pink },
		{ XMFLOAT3(0.6f, 0.f, +0.0f), green },
		{ XMFLOAT3(0.35f, 0.f, +0.0f), red }
	};
	
	
	unsigned int indices[] = { 0, 1, 2 };
	unsigned int indices2[] = { 0, 1, 2, 0, 2, 3 };
	unsigned int indices3[] = { 0, 1, 2, 1, 3, 4, 2, 1, 4, 2, 4, 5 };
	
	triangleShape = new Mesh(vertices, 3, indices, 3, device.Get());
	squareShape = new Mesh(vertices2, 4, indices2, 6, device.Get());
	houseShape = new Mesh(vertices3, 6, indices3, 12, device.Get());
}


// --------------------------------------------------------
// Handle resizing DirectX "stuff" to match the new window size.
// For instance, updating our projection matrix's aspect ratio.
// --------------------------------------------------------
void Game::OnResize()
{
	// Handle base-level DX resize stuff
	DXCore::OnResize();
}

// --------------------------------------------------------
// Update your game here - user input, move objects, AI, etc.
// --------------------------------------------------------
void Game::Update(float deltaTime, float totalTime)
{
	// Quit if the escape key is pressed
	if (GetAsyncKeyState(VK_ESCAPE))
		Quit();
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


	// Set the vertex and pixel shaders to use for the next Draw() command
	//  - These don't technically need to be set every frame
	//  - Once you start applying different shaders to different objects,
	//    you'll need to swap the current shaders before each draw
	context->VSSetShader(vertexShader.Get(), 0, 0);
	context->PSSetShader(pixelShader.Get(), 0, 0);


	// Ensure the pipeline knows how to interpret the data (numbers)
	// from the vertex buffer.  
	// - If all of your 3D models use the exact same vertex layout,
	//    this could simply be done once in Init()
	// - However, this isn't always the case (but might be for this course)
	context->IASetInputLayout(inputLayout.Get());

	VertexShaderExternalData vsData;
	
	vsData.colorTint = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vsData.offset = XMFLOAT3(0.25f, 0.0f, 0.0f);

	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};    
	context->Map(constantBufferVS.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);      
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));      
	context->Unmap(constantBufferVS.Get(), 0);

	 context->VSSetConstantBuffers( 0, 1, constantBufferVS.GetAddressOf());

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	//  - for this demo, this step *could* simply be done once during Init(),
	//    but I'm doing it here because it's often done multiple times per frame
	//    in a larger application/game
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// Draw a triangle
	context->IASetVertexBuffers(0, 1, triangleShape->GetVertexBuffer(), &stride, &offset);
	context->IASetIndexBuffer(triangleShape->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(
		triangleShape->GetIndexCount(),     
		0,  
		0); 

	// Draw a square
	context->IASetVertexBuffers(0, 1, squareShape->GetVertexBuffer(), &stride, &offset);
	context->IASetIndexBuffer(squareShape->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(
		squareShape->GetIndexCount(),
		0, 
		0);

	// Draw a house
	context->IASetVertexBuffers(0, 1, houseShape->GetVertexBuffer(), &stride, &offset);
	context->IASetIndexBuffer(houseShape->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed(
		houseShape->GetIndexCount(),
		0, 
		0);

	// Present the back buffer to the user
	//  - Puts the final frame we're drawing into the window so the user can see it
	//  - Do this exactly ONCE PER FRAME (always at the very end of the frame)
	swapChain->Present(0, 0);

	// Due to the usage of a more sophisticated swap chain,
	// the render target must be re-bound after every call to Present()
	context->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}