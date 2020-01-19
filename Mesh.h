#pragma once

#include <wrl/client.h>
#include <d3d11.h>
#include "Vertex.h"

class Mesh
{
public:
	Mesh(Vertex* vertexData, unsigned int vertexCount, unsigned int* indices, int indexCount, ID3D11Device* device);
	Mesh(const Mesh& other);
	Mesh& operator=(const Mesh& other);
	~Mesh() = default;

	ID3D11Buffer* const* GetVertexBuffer() const;
	ID3D11Buffer* GetIndexBuffer() const;
	int GetIndexCount() const;
private:
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	
	int indexBufferCount = 0;
};
