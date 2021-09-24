#pragma once
#include "stdafx.h"

class Vertex
{
public:
	Vertex() = default;
	Vertex(const XMFLOAT3& position) : m_position{ position } { }
	~Vertex() = default;

public:
	XMFLOAT3 m_position;
};

class TextureVertex : public Vertex
{
public:
	TextureVertex() = default;
	TextureVertex(const XMFLOAT3& position, const XMFLOAT2& uv) : Vertex{ position }, m_uv{ uv } { }
	~TextureVertex() = default;

public:
	XMFLOAT2 m_uv;
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<Vertex>& vertices, const vector<UINT>& indices);
	Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<TextureVertex>& vertices, const vector<UINT>& indices);
	~Mesh() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList) const;
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
};

class CubeMesh : public Mesh
{
public:
	CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT x, FLOAT y, FLOAT z);
	~CubeMesh() = default;
};