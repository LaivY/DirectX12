#pragma once
#include "stdafx.h"
#include "texture.h"

class Shader
{
public:
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature, const shared_ptr<Texture>& texture);
	~Shader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	virtual void CreateSrvDescriptorHeap(const ComPtr<ID3D12Device>& device);
	virtual void CreateShaderResourceView(const ComPtr<ID3D12Device>& device, const shared_ptr<Texture>& texture = NULL);

	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }
	ComPtr<ID3D12DescriptorHeap> GetSrvHeap() const { return m_srvHeap; }

protected:
	ComPtr<ID3D12PipelineState>		m_pipelineState;
	ComPtr<ID3D12DescriptorHeap>	m_srvHeap;
};