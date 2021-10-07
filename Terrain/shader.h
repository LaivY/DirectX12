#pragma once
#include "stdafx.h"
#include "texture.h"

class Shader
{
public:
	Shader() = default;
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~Shader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }

protected:
	ComPtr<ID3D12PipelineState>		m_pipelineState;
};

class SkyboxShader : public Shader
{
public:
	SkyboxShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	~SkyboxShader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
};