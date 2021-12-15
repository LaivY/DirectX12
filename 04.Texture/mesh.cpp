#include "mesh.h"

Mesh::Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<Vertex>& vertices, const vector<UINT>& indices)
{
	// ���� ���� ���� ����
	m_nVertices = vertices.size();

	// ���� ���� ����
	m_vertexBuffer = CreateBufferResource(device, commandList, vertices.data(), sizeof(Vertex), vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_vertexUploadBuffer);

	// ���� ���� �� ����
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(Vertex) * vertices.size();
	m_vertexBufferView.StrideInBytes = sizeof(Vertex);

	//--------------------------------------------------------------------

	// �ε��� ���� ���� ����
	m_nIndices = indices.size();

	// �ε��� �����Ͱ� �ִٸ� ����
	if (m_nIndices)
	{
		// �ε��� ���� ����
		m_indexBuffer = CreateBufferResource(device, commandList, indices.data(), sizeof(UINT), indices.size(),
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_indexUploadBuffer);

		// �ε��� ���� �� ����
		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = sizeof(UINT) * indices.size();
	}
}

Mesh::Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const vector<TextureVertex>& vertices, const vector<UINT>& indices)
{
	m_nVertices = vertices.size();
	m_vertexBuffer = CreateBufferResource(device, commandList, vertices.data(), sizeof(TextureVertex), vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_vertexUploadBuffer);
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(TextureVertex) * vertices.size();
	m_vertexBufferView.StrideInBytes = sizeof(TextureVertex);

	//--------------------------------------------------------------------

	m_nIndices = indices.size();
	if (m_nIndices)
	{
		m_indexBuffer = CreateBufferResource(device, commandList, indices.data(), sizeof(UINT), indices.size(),
			D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_indexUploadBuffer);
		m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = sizeof(UINT) * indices.size();
	}
}

void Mesh::Render(const ComPtr<ID3D12GraphicsCommandList>& m_commandList) const
{
	m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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

void Mesh::ReleaseUploadBuffer()
{
	if (m_vertexUploadBuffer) m_vertexUploadBuffer.Reset();
	if (m_indexUploadBuffer) m_indexUploadBuffer.Reset();
}

CubeMesh::CubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT x, FLOAT y, FLOAT z)
{
	// ť�� ����, ����, ����
	FLOAT sx{ x }, sy{ y }, sz{ z };

	// �ո�
	vector<TextureVertex> vertices;
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	// �����ʸ�
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	// ���ʸ�
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 0.0f, 1.0f });

	// �޸�
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 0.0f, 1.0f });

	// ����
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ -sx, +sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, +sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, +sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	// �ظ�
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, +sz }, XMFLOAT2{ 1.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });

	vertices.emplace_back(XMFLOAT3{ +sx, -sy, +sz }, XMFLOAT2{ 0.0f, 0.0f });
	vertices.emplace_back(XMFLOAT3{ -sx, -sy, -sz }, XMFLOAT2{ 1.0f, 1.0f });
	vertices.emplace_back(XMFLOAT3{ +sx, -sy, -sz }, XMFLOAT2{ 0.0f, 1.0f });

	m_nVertices = vertices.size();
	m_vertexBuffer = CreateBufferResource(device, commandList, vertices.data(), sizeof(TextureVertex), vertices.size(),
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, m_vertexUploadBuffer);
	m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.SizeInBytes = sizeof(TextureVertex) * vertices.size();
	m_vertexBufferView.StrideInBytes = sizeof(TextureVertex);
}