#include "Entity.h"
#include "Mesh.h"
#include "BufferStructs.h"

Entity::Entity(Mesh* incomingMesh)
{
	mesh = incomingMesh;
}

Mesh* Entity::GetMesh() const
{
	return mesh;
}

Transform* Entity::GetTransform()
{
	return &transform;
}

// doesn't involve instanced rendering yet
void Entity::Draw(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer)
{
	// set the vertex shader data
	VertexShaderExternalData vsData;
	vsData.colorTint = DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	vsData.worldMatrix = transform.GetWorldMatrix();

	// map the vertex shader data to the constant buffer
	D3D11_MAPPED_SUBRESOURCE mappedBuffer = {};
	context->Map(constantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedBuffer);
	memcpy(mappedBuffer.pData, &vsData, sizeof(vsData));
	context->Unmap(constantBuffer, 0);
	context->VSSetConstantBuffers(0, 1, &constantBuffer);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	// set vertex and index buffers and draw the mesh
	context->IASetVertexBuffers(0, 1, mesh->GetVertexBuffer(), &stride, &offset);
	context->IASetIndexBuffer(mesh->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
	context->DrawIndexed
	(
		mesh->GetIndexCount(),
		0,
		0
	);
}
