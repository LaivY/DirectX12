#pragma once
#include "stdafx.h"
#include "mesh.h"
#include "shader.h"
#include "texture.h"

class Camera;

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Update(FLOAT deltaTime) { }
	virtual void Move(const XMFLOAT3& shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);
	virtual void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void SetPosition(const XMFLOAT3& position);
	void SetMesh(const shared_ptr<Mesh>& Mesh);
	void SetShader(const shared_ptr<Shader>& shader);
	void SetTexture(const shared_ptr<Texture>& texture);

	XMFLOAT3 GetPosition() const;
	XMFLOAT3 GetRight() const { return m_right; }
	XMFLOAT3 GetUp() const { return m_up; }
	XMFLOAT3 GetFront() const { return m_front; }

	void ReleaseUploadBuffer() const;

protected:
	XMFLOAT4X4				m_worldMatrix;		// ���� ��ȯ

	XMFLOAT3				m_right;			// ���� x��
	XMFLOAT3				m_up;				// ���� y��
	XMFLOAT3				m_front;			// ���� z��

	FLOAT					m_roll;				// z�� ȸ����
	FLOAT					m_pitch;			// x�� ȸ����
	FLOAT					m_yaw;				// y�� ȸ����

	XMFLOAT3				m_normal;			// ���� ��ġ�� ������ ��� ����	(�ӽ�)

	shared_ptr<Mesh>		m_mesh;				// �޽�
	shared_ptr<Shader>		m_shader;			// ���̴�
	shared_ptr<Texture>		m_texture;			// �ؽ���
};

class BillboardObject : public GameObject
{
public:
	BillboardObject(const shared_ptr<Camera>& camera);
	~BillboardObject() = default;

	virtual void Update(FLOAT deltaTime);
	void SetCamera(const shared_ptr<Camera>& camera);

private:
	shared_ptr<Camera>	m_camera; // ������ �Ǵ� ī�޶�
};