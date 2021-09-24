#pragma once
#include "stdafx.h"
#include "DDSTextureLoader12.h"

class Texture
{
public:
	Texture(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);
	~Texture() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList);
	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);
	void ReleaseUploadBuffer();

private:
	ComPtr<ID3D12Resource>			m_texture;
	ComPtr<ID3D12Resource>			m_textureUploadBuffer;

	ComPtr<ID3D12DescriptorHeap>	m_srvHeap;
};