#pragma once
#include "stdafx.h"
#include "object.h"

class Skybox
{
public:
	Skybox(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const ComPtr<ID3D12RootSignature>& rootSignature);
	~Skybox() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void SetPosition(XMFLOAT3 position);

private:
	unique_ptr<GameObject[]> m_faces;
};