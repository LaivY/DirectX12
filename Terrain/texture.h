#pragma once
#include "stdafx.h"
#include "DDSTextureLoader12.h"

class Texture
{
public:
	Texture(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);
	~Texture() = default;

	void LoadTextureFile(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const wstring& fileName);
	void CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	void CreateShaderResourceView(const ComPtr<ID3D12Device>& device);
	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList);
	void ReleaseUploadBuffer();

	ComPtr<ID3D12DescriptorHeap> GetSrvHeap() const { return m_srvHeap; }
	ComPtr<ID3D12Resource> GetTexture() const { return m_texture; }

private:
	ComPtr<ID3D12Resource>			m_texture;
	ComPtr<ID3D12Resource>			m_textureUploadBuffer;
	ComPtr<ID3D12DescriptorHeap>	m_srvHeap;
};