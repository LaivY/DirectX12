#pragma once
#include "stdafx.h"
#include "mesh.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Update() {}
	virtual void Move(const XMFLOAT3& shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	XMFLOAT4X4 GetWorldMatrix() const { return m_worldMatrix; }
	XMFLOAT3 GetPosition() const { return XMFLOAT3{ m_worldMatrix._41, m_worldMatrix._42, m_worldMatrix._43 }; }
	XMFLOAT3 GetRight() const { return m_right; }
	XMFLOAT3 GetUp() const { return m_up; }
	XMFLOAT3 GetFront() const { return m_front; }
	BoundingOrientedBox GetBoundingBox() const { return m_boundingBox; }

	void SetPosition(const XMFLOAT3& position);
	void SetMesh(const Mesh& pMesh);
	void SetBoundingBox(const BoundingOrientedBox& boundingBox) { m_boundingBox = boundingBox; }

	void ReleaseMeshUploadBuffer() const { if (m_mesh) m_mesh->ReleaseUploadBuffer(); }

protected:
	XMFLOAT4X4			m_worldMatrix;	// 월드 변환

	XMFLOAT3			m_right;		// 로컬 x축
	XMFLOAT3			m_up;			// 로컬 y축
	XMFLOAT3			m_front;		// 로컬 z축

	FLOAT				m_roll;			// x축 회전각
	FLOAT				m_pitch;		// y축 회전각
	FLOAT				m_yaw;			// z축 회전각

	unique_ptr<Mesh>	m_mesh;			// 메쉬
	BoundingOrientedBox	m_boundingBox;	// 바운딩박스
};