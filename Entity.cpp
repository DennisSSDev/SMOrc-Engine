#include "Entity.h"
#include "Mesh.h"
#include "Camera.h"
#include "Material.h"
#include "BufferStructs.h"

Entity::Entity(Mesh* incomingMesh, Material* incomingMaterial)
{
	mesh = incomingMesh;
	material = incomingMaterial;
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
void Entity::Draw(ID3D11DeviceContext* context, ID3D11Buffer* constantBuffer, Camera* mainCamera)
{
	context->VSSetShader(material->GetVertexShader(), 0, 0);
	context->PSSetShader(material->GetPixelShader(), 0, 0);

	// set the vertex shader data
	VertexShaderExternalData vsData;
	vsData.colorTint = material->GetColorTint();
	vsData.worldMatrix = transform.GetWorldMatrix();
	vsData.viewMatrix = mainCamera->GetViewMatrix();
	vsData.projMatrix = mainCamera->GetProjectionMatrix();

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
