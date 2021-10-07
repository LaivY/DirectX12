#include "mesh.h"

Mesh::Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	void* vertexData, UINT sizePerVertexData, UINT vertexDataCount, void* indexData, UINT indexDataCount, D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
	: m_primitiveTopology{ primitiveTopology }
{
	if (vertexData) CreateVertexBuffer(device, commandList, vertexData, sizePerVertexData, vertexDataCount);
	if (indexData) CreateIndexBuffer(device, commandList, indexData, indexDataCount);
}

void Mesh::Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList) const
{
	m_commandList->IASetPrimitiveTopology(m_primitiveTopology);
	m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	if (m_nIndices)
	{
		m_commandList->IASetIndexBuffer(&m_indexBufferView);
		m_commandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
	}
	else
	{
		m_commandList->DrawInstanced(m_nVertices, 1, 0, 0);
	}
}

void Mesh::CreateVertexBuffer(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, UINT sizePerData, UINT dataCount)
{
	// 정점 버퍼 갯수 저장
	m_nVertices = dataCount;

	// 정점 버퍼 생성
	m_vertexBuffer = CreateBufferResource(device, commandList, data, sizePerData, dataCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_vertexUploadBuffer);

	// 정점 버퍼 뷰 설정
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizePerData * dataCount;
	m_vertexBufferView.StrideInBytes = sizePerData;
}

void Mesh::CreateIndexBuffer(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, void* data, UINT dataCount)
{
	// 인덱스 버퍼 갯수 저장
	m_nIndices = dataCount;

	// 인덱스 버퍼 생성
	m_indexBuffer = CreateBufferResource(device, commandList, data, sizeof(UINT), dataCount, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_indexUploadBuffer);

	// 인덱스 버퍼 뷰 설정
	m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * dataCount;
}

void Mesh::ReleaseUploadBuffer()
{
	if (m_vertexUploadBuffer) m_vertexUploadBuffer.Reset();
	if (m_indexUploadBuffer) m_indexUploadBuffer.Reset();
}

CubeMesh::CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT width, FLOAT length, FLOAT height)
{
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 큐브 가로, 세로, 높이
	FLOAT sx{ width }, sy{ length }, sz{ height };

	// 앞면
	vector<TextureVertex> vertices;
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	// 오른쪽면
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	// 왼쪽면
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 0.0f, 1.0f });

	// 뒷면
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 0.0f, 1.0f });

	// 윗면
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	// 밑면
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	CreateVertexBuffer(device, commandList, vertices.data(), sizeof(TextureVertex), vertices.size());
}

RectMesh::RectMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT width, FLOAT height)
{
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	vector<TextureVertex> vertices;
	vertices.emplace_back(XMFLOAT3{ -width / 2.0f, +height / 2.0f, 0.0f }, XMFLOAT2{ 0.0f, 0.0f }); // LT
	vertices.emplace_back(XMFLOAT3{ +width / 2.0f, +height / 2.0f, 0.0f }, XMFLOAT2{ 1.0f, 0.0f }); // RT
	vertices.emplace_back(XMFLOAT3{ +width / 2.0f, -height / 2.0f, 0.0f }, XMFLOAT2{ 1.0f, 1.0f }); // RB
	vertices.emplace_back(XMFLOAT3{ -width / 2.0f, -height / 2.0f, 0.0f }, XMFLOAT2{ 0.0f, 1.0f }); // LB
	CreateVertexBuffer(device, commandList, vertices.data(), sizeof(TextureVertex), vertices.size());

	vector<UINT> indices;
	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(0); indices.push_back(2); indices.push_back(3);
	CreateIndexBuffer(device, commandList, indices.data(), indices.size());
}