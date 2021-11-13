#pragma once
#include "stdafx.h"
#include "texture.h"

class Shader
{
public:
	Shader() = default;
	Shader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature) { CreatePipelineState(device, rootSignature); }
	~Shader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }

protected:
	ComPtr<ID3D12PipelineState> m_pipelineState;
};

class TerrainShader : public Shader
{
public:
	TerrainShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature) { CreatePipelineState(device, rootSignature); }
	~TerrainShader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
};

class TerrainTessShader : public Shader
{
public:
	TerrainTessShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature) { CreatePipelineState(device, rootSignature); }
	~TerrainTessShader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
};

class TerrainTessWireShader : public Shader
{
public:
	TerrainTessWireShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature) { CreatePipelineState(device, rootSignature); }
	~TerrainTessWireShader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
};

class SkyboxShader : public Shader
{
public:
	SkyboxShader(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature) { CreatePipelineState(device, rootSignature); }
	~SkyboxShader() = default;

	virtual void CreatePipelineState(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
};