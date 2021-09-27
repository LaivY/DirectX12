#pragma once
#include "stdafx.h"
#include "DDSTextureLoader12.h"

class Texture
{
public:
	Texture(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);
	~Texture() = default;

	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList, D3D12_GPU_DESCRIPTOR_HANDLE srvGpuDescriptorHandle);
	void ReleaseUploadBuffer();

	ComPtr<ID3D12Resource> GetTexture() const { return m_texture; }
	D3D12_SHADER_RESOURCE_VIEW_DESC GetShaderResourceViewDesc() const;

private:
	ComPtr<ID3D12Resource> m_texture;
	ComPtr<ID3D12Resource> m_textureUploadBuffer;
};