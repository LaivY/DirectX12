#include "object.h"

GameObject::GameObject() : m_right{ 1.0f, 0.0f, 0.0f }, m_up{ 0.0f, 1.0f, 0.0f }, m_front{ 0.0f, 0.0f, 1.0f }, m_roll{ 0.0f }, m_pitch{ 0.0f }, m_yaw{ 0.0f }
{
	XMStoreFloat4x4(&m_worldMatrix, XMMatrixIdentity());
}

GameObject::~GameObject()
{
	m_mesh->ReleaseUploadBuffer();
}

void GameObject::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// 셰이더 변수 최신화
	UpdateShaderVariable(commandList);

	// 렌더링
	if (m_mesh) m_mesh->Render(commandList);
}

void GameObject::Move(const XMFLOAT3& shift)
{
	SetPosition(Vector3::Add(GetPosition(), shift));
}

void GameObject::Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw)
{
	// 회전
	XMMATRIX rotate{ XMMatrixRotationRollPitchYaw(XMConvertToRadians(roll), XMConvertToRadians(pitch), XMConvertToRadians(yaw)) };
	XMMATRIX worldMatrix{ rotate * XMLoadFloat4x4(&m_worldMatrix) };
	XMStoreFloat4x4(&m_worldMatrix, worldMatrix);

	// 로컬 x,y,z축 최신화
	XMStoreFloat3(&m_right, XMVector3TransformNormal(XMLoadFloat3(&m_right), rotate));
	XMStoreFloat3(&m_up, XMVector3TransformNormal(XMLoadFloat3(&m_up), rotate));
	XMStoreFloat3(&m_front, XMVector3TransformNormal(XMLoadFloat3(&m_front), rotate));
}

void GameObject::UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	// PSO 설정
	if (m_shader) commandList->SetPipelineState(m_shader->GetPipelineState().Get());

	// 게임오브젝트의 월드 변환 행렬 최신화
	XMFLOAT4X4 worldMatrix;
	XMStoreFloat4x4(&worldMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_worldMatrix)));
	commandList->SetGraphicsRoot32BitConstants(0, 16, &worldMatrix, 0);

	if (m_texture && m_shader)
	{
		ID3D12DescriptorHeap* ppHeaps[] = { m_shader->GetSrvHeap().Get() };
		commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);

		// GameFramework::CreateRootSignature에서 rootParameter[2]가 SRV 서술자 테이블임
		commandList->SetGraphicsRootDescriptorTable(2, m_shader->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart());
	}
}

void GameObject::SetPosition(const XMFLOAT3& position)
{
	m_worldMatrix._41 = position.x;
	m_worldMatrix._42 = position.y;
	m_worldMatrix._43 = position.z;
}

void GameObject::SetMesh(const shared_ptr<Mesh>& mesh)
{
	if (m_mesh) m_mesh.reset();
	m_mesh = mesh;
}

void GameObject::SetShader(const shared_ptr<Shader>& shader)
{
	if (m_shader) m_shader.reset();
	m_shader = shader;
}

void GameObject::SetTexture(const shared_ptr<Texture>& texture)
{
	if (m_texture) m_texture.reset();
	m_texture = texture;
}

XMFLOAT3 GameObject::GetPosition() const
{
	return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 };
}

void GameObject::ReleaseUploadBuffer() const
{
	if (m_mesh) m_mesh->ReleaseUploadBuffer();
	if (m_texture) m_texture->ReleaseUploadBuffer();
}