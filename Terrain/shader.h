#pragma once
#include "stdafx.h"
#include "texture.h"

class Shader
{
public:
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~Shader() = default;

	void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	void CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	void CreateShaderResourceView(const ComPtr<ID3D12Device>& device, const shared_ptr<Texture>& texture);
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }
	ComPtr<ID3D12DescriptorHeap> GetSrvHeap() const { return m_srvHeap; }

protected:
	ComPtr<ID3D12PipelineState>		m_pipelineState;
	ComPtr<ID3D12DescriptorHeap>	m_srvHeap;
};