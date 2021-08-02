#pragma once
#include "stdafx.h"
#include "mesh.h"

struct Instance
{
	XMFLOAT4X4 worldMatrix;
};

class Instancing
{
public:
	Instancing(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature, const Mesh& mesh, UINT sizeofData, UINT count);
	~Instancing();

	void CreatePipelineStateObject(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12RootSignature>& rootSignature);
	void CreateInstanceBuffer(const ComPtr<ID3D12Device>& device);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	ComPtr<ID3D12PipelineState> GetPipelineState() const { return m_pipelineState; }
	ComPtr<ID3D12Resource> GetInstanceBuffer() const { return m_instanceBuffer; }
	Instance* GetInstancePointer() const { return m_instanceBufferPointer; }

private:
	unique_ptr<Mesh>			m_mesh;
	ComPtr<ID3D12PipelineState>	m_pipelineState;

	UINT						m_sizeInBytes;
	UINT						m_strideInBytes;
	ComPtr<ID3D12Resource>		m_instanceBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_instanceBufferView;
	Instance*					m_instanceBufferPointer;
};