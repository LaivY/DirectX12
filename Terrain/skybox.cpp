#include "skybox.h"

Skybox::Skybox(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const ComPtr<ID3D12RootSignature>& rootSignature) : m_faces{ new GameObject[6] }
{
	// �޽� ����
	shared_ptr<TextureRectMesh> frontMesh{ make_shared<TextureRectMesh>(device, commandList, 20.0f, 0.0f, 20.0f, XMFLOAT3{ 0.0f, 0.0f, 10.0f }) };
	shared_ptr<TextureRectMesh> leftMesh{ make_shared<TextureRectMesh>(device, commandList, 0.0f, 20.0f, 20.0f, XMFLOAT3{ -10.0f, 0.0f, 0.0f }) };
	shared_ptr<TextureRectMesh> rightMesh{ make_shared<TextureRectMesh>(device, commandList, 0.0f, 20.0f, 20.0f, XMFLOAT3{ 10.0f, 0.0f, 0.0f }) };
	shared_ptr<TextureRectMesh> backMesh{ make_shared<TextureRectMesh>(device, commandList, 20.0f, 0.0f, 20.0f, XMFLOAT3{ 0.0f, 0.0f, -10.0f }) };
	shared_ptr<TextureRectMesh> topMesh{ make_shared<TextureRectMesh>(device, commandList, 20.0f, 20.0f, 0.0f, XMFLOAT3{ 0.0f, 10.0f, 0.0f }) };
	shared_ptr<TextureRectMesh> botMesh{ make_shared<TextureRectMesh>(device, commandList, 20.0f, 20.0f, 0.0f, XMFLOAT3{ 0.0f, -10.0f, 0.0f }) };

	// ���̴� ����
	shared_ptr<SkyboxShader> skyboxShader{ make_shared<SkyboxShader>(device, rootSignature) };

	// �ؽ��� ����
	shared_ptr<Texture> frontTexture{ make_shared<Texture>(device, commandList, TEXT("resource/SkyboxFront.dds")) };
	shared_ptr<Texture> leftTexture{ make_shared<Texture>(device, commandList, TEXT("resource/SkyboxLeft.dds")) };
	shared_ptr<Texture> rightTexture{ make_shared<Texture>(device, commandList, TEXT("resource/SkyboxRight.dds")) };
	shared_ptr<Texture> backTexture{ make_shared<Texture>(device, commandList, TEXT("resource/SkyboxBack.dds")) };
	shared_ptr<Texture> topTexture{ make_shared<Texture>(device, commandList, TEXT("resource/SkyboxTop.dds")) };
	shared_ptr<Texture> botTexture{ make_shared<Texture>(device, commandList, TEXT("resource/SkyboxBot.dds")) };

	// �� ����
	m_faces[0].SetMesh(frontMesh);	m_faces[0].SetShader(skyboxShader); m_faces[0].SetTexture(frontTexture);	// ��
	m_faces[1].SetMesh(leftMesh);	m_faces[1].SetShader(skyboxShader); m_faces[1].SetTexture(leftTexture);		// ����
	m_faces[2].SetMesh(rightMesh);	m_faces[2].SetShader(skyboxShader); m_faces[2].SetTexture(rightTexture);	// ������
	m_faces[3].SetMesh(backMesh);	m_faces[3].SetShader(skyboxShader); m_faces[3].SetTexture(backTexture);		// ��
	m_faces[4].SetMesh(topMesh);	m_faces[4].SetShader(skyboxShader); m_faces[4].SetTexture(topTexture);		// ��
	m_faces[5].SetMesh(botMesh);	m_faces[5].SetShader(skyboxShader); m_faces[5].SetTexture(botTexture);		// �Ʒ�
}

void Skybox::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (int i = 0; i < 6; ++i)
		m_faces[i].Render(commandList);
}

void Skybox::SetPosition(XMFLOAT3 position)
{
	for (int i = 0; i < 6; ++i)
		m_faces[i].SetPosition(position);
}