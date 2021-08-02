#include "object.h"

GameObject::GameObject() : m_right{ 1.0f, 0.0f, 0.0f }, m_up{ 0.0f, 1.0f, 0.0f }, m_front{ 0.0f, 0.0f, 1.0f }, m_roll{ 0.0f }, m_pitch{ 0.0f }, m_yaw{ 0.0f }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
	m_boundingBox = BoundingOrientedBox(XMFLOAT3{ 0.0f, 0.0f, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, 0.0f }, XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f });
}

GameObject::~GameObject()
{
	if (m_mesh) m_mesh->ReleaseUploadBuffer();
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// ���̴� ���� ��ȯ ��� �ֽ�ȭ
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &worldMatrix, 0);

	// ������
	if (m_mesh) m_mesh->Render(commandList);
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

	// �ٿ�� �ڽ� ȸ�� (�������� �̵����״ٰ� ȸ�� �� �ٽ� ���ڸ��� �̵�)
	XMFLOAT3 origin{ m_boundingBox.Center };
	m_boundingBox.Transform(m_boundingBox, XMMatrixTranslation(-origin.x, -origin.y, -origin.z) * rotate * XMMatrixTranslation(origin.x, origin.y, origin.z));

	// ���� x,y,z�� �ֽ�ȭ
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
}

void GameObject::SetMesh(const Mesh& mesh)
{
	if (m_mesh) m_mesh.reset();
	m_mesh = make_unique<Mesh>(mesh);
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	// �ٿ���ڽ� �̵�
	m_boundingBox.Transform(m_boundingBox, XMMatrixTranslation(position.x - m_worldMatrix._41, position.y - m_worldMatrix._42, position.z - m_worldMatrix._43));

	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}