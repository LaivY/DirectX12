#pragma once
#include "stdafx.h"
#include "mesh.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList, const D3D12_VERTEX_BUFFER_VIEW& instanceBufferView) const;
	virtual void Update() { };
	virtual void Move(const XMFLOAT3& shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	XMFLOAT4X4 GetWorldMatrix() const { return m_worldMatrix; }

	void SetMesh(const Mesh& pMesh);
	XMFLOAT3 GetPosition() const;
	void SetPosition(const XMFLOAT3& position);

	XMFLOAT3 GetRight() const { return m_right; }
	XMFLOAT3 GetUp() const { return m_up; }
	XMFLOAT3 GetFront() const { return m_front; }

	void ReleaseMeshUploadBuffer() const { if (m_Mesh) m_Mesh->ReleaseUploadBuffer(); }

protected:
	XMFLOAT4X4			m_worldMatrix;	// ���� ��ȯ

	XMFLOAT3			m_right;		// ���� x��
	XMFLOAT3			m_up;			// ���� y��
	XMFLOAT3			m_front;		// ���� z��

	FLOAT				m_roll;			// x�� ȸ����
	FLOAT				m_pitch;		// y�� ȸ����
	FLOAT				m_yaw;			// z�� ȸ����

	unique_ptr<Mesh>	m_Mesh;			// �޽�
};