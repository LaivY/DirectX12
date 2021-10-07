#include "skybox.h"

Skybox::Skybox(const shared_ptr<RectMesh>& frontRectMesh, const shared_ptr<Shader>& shader,
	const shared_ptr<Texture>& frontTexture, const shared_ptr<Texture>& leftTexture, const shared_ptr<Texture>& rightTexture,
	const shared_ptr<Texture>& backTexture, const shared_ptr<Texture>& topTexture, const shared_ptr<Texture>& botTexture, const XMFLOAT3& position, FLOAT distance)
	: m_faces{ new GameObject[6] }, m_distance{ distance }
{
	for (int i = 0; i < 6; ++i)
	{
		m_faces[i].SetMesh(frontRectMesh);
		m_faces[i].SetShader(shader);
	}

	// ��(+z��)
	m_faces[0].SetTexture(frontTexture);
	m_faces[0].SetPosition(Vector3::Add(position, XMFLOAT3{ 0.0f, 0.0f, distance }));

	// ����(-x��)
	m_faces[1].SetTexture(leftTexture);
	m_faces[1].Rotate(0.0f, 0.0f, -90.0f);
	m_faces[1].SetPosition(Vector3::Add(position, XMFLOAT3{ -distance, 0.0f, 0.0f }));

	// ������(+x��)
	m_faces[2].SetTexture(rightTexture);
	m_faces[2].Rotate(0.0f, 0.0f, +90.0f);
	m_faces[2].SetPosition(Vector3::Add(position, XMFLOAT3{ distance, 0.0f, 0.0f }));

	// ��(-z��)
	m_faces[3].SetTexture(backTexture);
	m_faces[3].Rotate(0.0f, 0.0f, 180.0f);
	m_faces[3].SetPosition(Vector3::Add(position, XMFLOAT3{ 0.0f, 0.0f, -distance }));

	// ��(+y��)
	m_faces[4].SetTexture(topTexture);
	m_faces[4].Rotate(0.0f, -90.0f, 0.0f);
	m_faces[4].SetPosition(Vector3::Add(position, XMFLOAT3{ 0.0f, distance, 0.0f }));

	// �Ʒ�(-y��)
	m_faces[5].SetTexture(botTexture);
	m_faces[5].Rotate(0.0f, 90.0f, 0.0f);
	m_faces[5].SetPosition(Vector3::Add(position, XMFLOAT3{ 0.0f, -distance, 0.0f }));
}

void Skybox::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (int i = 0; i < 6; ++i)
		m_faces[i].Render(commandList);
}

void Skybox::Update(const XMFLOAT3& cameraPosition)
{
	m_faces[0].SetPosition(Vector3::Add(cameraPosition, XMFLOAT3{ 0.0f, 0.0f, m_distance }));
	m_faces[1].SetPosition(Vector3::Add(cameraPosition, XMFLOAT3{ -m_distance, 0.0f, 0.0f }));
	m_faces[2].SetPosition(Vector3::Add(cameraPosition, XMFLOAT3{ m_distance, 0.0f, 0.0f }));
	m_faces[3].SetPosition(Vector3::Add(cameraPosition, XMFLOAT3{ 0.0f, 0.0f, -m_distance }));
	m_faces[4].SetPosition(Vector3::Add(cameraPosition, XMFLOAT3{ 0.0f, m_distance, 0.0f }));
	m_faces[5].SetPosition(Vector3::Add(cameraPosition, XMFLOAT3{ 0.0f, -m_distance, 0.0f }));
}