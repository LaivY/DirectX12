#include "object.h"

GameObject::GameObject() : m_right{ 1.0f, 0.0f, 0.0f }, m_up{ 0.0f, 1.0f, 0.0f }, m_front{ 0.0f, 0.0f, 1.0f }, m_roll{ 0.0f }, m_pitch{ 0.0f }, m_yaw{ 0.0f }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	m_Mesh->ReleaseUploadBuffer();
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// ���̴� ���� ��ȯ ��� �ֽ�ȭ
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &worldMatrix, 0);

	// ������
	if (m_Mesh) m_Mesh->Render(commandList);
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList, const D3D12_VERTEX_BUFFER_VIEW& instanceBufferView) const
{
	if (m_Mesh) m_Mesh->Render(commandList, instanceBufferView);
}

void GameObject::Move(const XMFLOAT3& shift)
{
	SetPosition(Vector3::Add(GetPosition(), shift));
}

void GameObject::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	// ȸ��
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(roll), XMConvertToRadians(pitch), XMConvertToRadians(yaw)) };
	XMMATRIX worldMatrix{ rotate * XMLoadFloat4x4(&m_worldMatrix) };
	XMStoreFloat4x4(&m_worldMatrix, worldMatrix);

	// ���� x,y,z�� �ֽ�ȭ
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
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