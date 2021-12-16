#pragma once
#include "stdafx.h"

struct Vertex
{
	XMFLOAT3 position{};
	XMFLOAT3 normal{};
	XMFLOAT4 color{};
	XMFLOAT2 uv0{};
	XMFLOAT2 uv1{};
};

struct ParticleVertex
{
	XMFLOAT3 position{};
	XMFLOAT2 size{};
	FLOAT lifeTime{};
	FLOAT age{};
};

class Mesh
{
public:
	Mesh() = default;
	Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		void* vertexData, UINT sizePerVertexData, UINT vertexDataCount, void* indexData, UINT indexDataCount, D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const string& fileName, D3D_PRIMITIVE_TOPOLOGY primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	~Mesh() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList) const;
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList, const D3D12_VERTEX_BUFFER_VIEW& instanceBufferView, UINT count) const;
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

class ReverseCubeMesh : public Mesh
{
public:
	ReverseCubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT width, FLOAT length, FLOAT height);
	~ReverseCubeMesh() = default;
};

class TextureRectMesh : public Mesh
{
public:
	TextureRectMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT width, FLOAT length, FLOAT height, XMFLOAT3 position);
	~TextureRectMesh() = default;
};

class BillboardMesh : public Mesh
{
public:
	BillboardMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const XMFLOAT3& position, const XMFLOAT2& size);
	~BillboardMesh() = default;
};