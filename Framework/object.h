#pragma once
#include "stdafx.h"
#include "mesh.h"

class GameObject
{
public:
	GameObject();
	~GameObject();

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void Update() { };

	void SetMesh(const Mesh& pMesh);
	XMFLOAT3 GetPosition() const;
	void SetPosition(const XMFLOAT3& position);

	void ReleaseMeshUploadBuffer() const { m_Mesh->ReleaseUploadBuffer(); }

protected:
	XMFLOAT4X4			m_worldMatrix;
	unique_ptr<Mesh>	m_Mesh;
};