#pragma once
#include "stdafx.h"
#include "object.h"

class Skybox
{
public:
	Skybox(const shared_ptr<RectMesh>& frontRectMesh, const shared_ptr<Shader>& shader,
		const shared_ptr<Texture>& frontTexture, const shared_ptr<Texture>& leftTexture, const shared_ptr<Texture>& rightTexture,
		const shared_ptr<Texture>& backTexture, const shared_ptr<Texture>& topTexture, const shared_ptr<Texture>& botTexture, const XMFLOAT3& position, FLOAT distance);
	~Skybox() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void Update(const XMFLOAT3& cameraPosition);

private:
	unique_ptr<GameObject[]>	m_faces;
	FLOAT						m_distance;
};