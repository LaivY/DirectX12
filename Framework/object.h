#pragma once
#include "stdafx.h"
#include "mesh.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	virtual void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	virtual void Update() { };
	virtual void Move(const XMFLOAT3& shift);
	virtual void Rotate(FLOAT roll, FLOAT pitch, FLOAT yaw);

	void SetMesh(const Mesh& pMesh);
	XMFLOAT3 GetPosition() const;
	void SetPosition(const XMFLOAT3& position);

	void ReleaseMeshUploadBuffer() const { m_Mesh->ReleaseUploadBuffer(); }

protected:
	XMFLOAT4X4			m_worldMatrix;
	FLOAT				m_roll;
	FLOAT				m_pitch;
	FLOAT				m_yaw;

	unique_ptr<Mesh>	m_Mesh;
};