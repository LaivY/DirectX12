#pragma once
#include "stdafx.h"

class Vertex
{
public:
	Vertex(const XMFLOAT3& position) : m_position{ position } { }
	~Vertex() = default;

protected:
	XMFLOAT3 m_position;
};

class TextureVertex : public Vertex
{
public:
	TextureVertex(const XMFLOAT3& position, const XMFLOAT2& uv) : Vertex{ position }, m_uv{ uv } { }
	~TextureVertex() = default;

private:
	XMFLOAT2 m_uv;
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		void* vertexData, UINT sizePerVertexData, UINT vertexDataCount, void* indexData, UINT indexDataCount, D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	~Mesh() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList) const;
	void CreateVertexBuffer(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, UINT sizePerData, UINT dataCount);
	void CreateIndexBuffer(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, UINT dataCount);
	void ReleaseUploadBuffer();

protected:
	UINT						m_nVertices;
	ComPtr<ID3D12Resource>		m_vertexBuffer;
	ComPtr<ID3D12Resource>		m_vertexUploadBuffer;
	D3D12_VERTEX_BUFFER_VIEW	m_vertexBufferView;

	UINT						m_nIndices;
	ComPtr<ID3D12Resource>		m_indexBuffer;
	ComPtr<ID3D12Resource>		m_indexUploadBuffer;
	D3D12_INDEX_BUFFER_VIEW		m_indexBufferView;

	D3D_PRIMITIVE_TOPOLOGY		m_primitiveTopology;
};

class CubeMesh : public Mesh
{
public:
	CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT width, FLOAT length, FLOAT height);
	~CubeMesh() = default;
};