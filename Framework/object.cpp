#include "object.h"

GameObject::GameObject()
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	m_Mesh->ReleaseUploadBuffer();
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// 셰이더 월드 변환 행렬 최신화
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &worldMatrix, 0);

	// 렌더링
	if (m_Mesh) m_Mesh->Render(commandList);
}

void GameObject::SetMesh(const Mesh& mesh)
{
	if (m_Mesh) m_Mesh.reset();
	m_Mesh = make_unique<Mesh>(mesh);
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 };
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}