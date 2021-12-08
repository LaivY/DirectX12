#include "mesh.h"

Mesh::Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	void* vertexData, UINT sizePerVertexData, UINT vertexDataCount, void* indexData, UINT indexDataCount, D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
	: m_nVertices{ vertexDataCount }, m_nIndices{ indexDataCount }, m_primitiveTopology{ primitiveTopology }
{
	if (vertexData) CreateVertexBuffer(device, commandList, vertexData, sizePerVertexData, vertexDataCount);
	if (indexData) CreateIndexBuffer(device, commandList, indexData, indexDataCount);
}

Mesh::Mesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const string& fileName, D3D_PRIMITIVE_TOPOLOGY primitiveTopology)
	: m_nIndices{ 0 }, m_primitiveTopology{ primitiveTopology }
{
	vector<Vertex> vertices;
	vector<XMFLOAT3> positions;
	vector<XMFLOAT3> normals;
	vector<XMFLOAT4> colors;

	ifstream file{ fileName };
	if (!file)
	{
		cout << "오류!" << endl;
		exit(-1);
	}

	string line;
	while (getline(file, line))
	{
		stringstream ss{ line };
		if (line[0] == 'v' && line[1] == 'n')
		{
			char type;			ss >> type >> type;
			XMFLOAT3 normal;	ss >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (line[0] == 'v')
		{
			char type;		ss >> type;
			XMFLOAT3 pos;	ss >> pos.x >> pos.y >> pos.z;
			positions.push_back(pos);
		}
		else if (line[0] == 'f')
		{
			replace(line.begin(), line.end(), '/', ' ');
			ss = stringstream{ line };

			char type; ss >> type;
			for (int i = 0; i < 3; ++i)
			{
				int vi, vni; ss >> vi >> vni;

				Vertex v;
				v.position	= positions[vi - 1];
				v.normal	= normals[vni - 1];
				v.color.x	= static_cast<float>(rand());
				v.color.y	= static_cast<float>(rand());
				v.color.z	= static_cast<float>(rand());
				v.color.w	= 1.0f;
				vertices.push_back(v);
			}
		}
	}
	CreateVertexBuffer(device, commandList, vertices.data(), sizeof(Vertex), vertices.size());
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
	else m_commandList->DrawInstanced(m_nVertices, 1, 0, 0);
}

void Mesh::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList, const D3D12_VERTEX_BUFFER_VIEW& instanceBufferView, UINT count) const
{
	commandList->IASetPrimitiveTopology(m_primitiveTopology);
	commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
	commandList->IASetVertexBuffers(1, 1, &instanceBufferView);
	if (m_nIndices)
	{
		commandList->IASetIndexBuffer(&m_indexBufferView);
		commandList->DrawIndexedInstanced(m_nIndices, count, 0, 0, 0);
	}
	else commandList->DrawInstanced(m_nVertices, count, 0, 0);
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
	m_nIndices = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 큐브 가로, 세로, 높이
	FLOAT sx{ width }, sy{ length }, sz{ height };

	Vertex v;
	vector<Vertex> vertices(36);

	// 앞면
	v.position = { -sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, -sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { -sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 오른쪽면
	v.position = { +sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { +sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 왼쪽면
	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, +sy, -sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 뒷면
	v.position = { +sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, +sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { +sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, +sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 윗면
	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { -sx, +sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 밑면
	v.position = { +sx, -sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { +sx, -sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	CreateVertexBuffer(device, commandList, vertices.data(), sizeof(Vertex), vertices.size());
}

ReverseCubeMesh::ReverseCubeMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT width, FLOAT length, FLOAT height)
{
	m_nIndices = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	// 큐브 가로, 세로, 높이
	FLOAT sx{ width }, sy{ length }, sz{ height };

	// 앞면
	Vertex v;
	vector<Vertex> vertices(36);

	// 앞면
	v.position = { -sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, -sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { -sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 오른쪽면
	v.position = { +sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { +sx, +sy, -sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 왼쪽면
	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, +sy, -sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 뒷면
	v.position = { +sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, +sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { +sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, +sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 윗면
	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { -sx, +sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { +sx, +sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { -sx, +sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 밑면
	v.position = { +sx, -sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, +sz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

	v.position = { +sx, -sy, +sz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
	v.position = { -sx, -sy, -sz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
	v.position = { +sx, -sy, -sz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);

	// 큐브 메쉬의 정점 순서를 거꾸로하면 안밖이 바뀜
	std::reverse(vertices.begin(), vertices.end());

	CreateVertexBuffer(device, commandList, vertices.data(), sizeof(Vertex), vertices.size());
}

TextureRectMesh::TextureRectMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, FLOAT width, FLOAT length, FLOAT height, XMFLOAT3 position)
{
	m_nIndices = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	Vertex v;
	vector<Vertex> vertices(6);
	FLOAT hx{ position.x + width / 2.0f }, hy{ position.y + height / 2.0f }, hz{ position.z + length / 2.0f };
	
	if (width == 0.0f)
	{
		// 분기하는 이유는 위치에 따라 삼각형 와인딩 순서가 달라지기 때문에
		// YZ평면
		if (position.x > 0.0f)
		{
			v.position = { +hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, +hy, -hz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, -hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

			v.position = { +hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, -hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, +hz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);
		}
		else
		{
			v.position = { +hx, +hy, -hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, +hy, +hz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

			v.position = { +hx, +hy, -hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, -hz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);
		}
	}
	else if (length == 0.0f)
	{
		// XY평면
		if (position.z > 0.0f)
		{
			v.position = { -hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, +hy, +hz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

			v.position = { -hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
			v.position = { -hx, -hy, +hz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);
		}
		else
		{
			v.position = { +hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { -hx, +hy, +hz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
			v.position = { -hx, -hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

			v.position = { +hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { -hx, -hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
			v.position = { +hx, -hy, +hz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);
		}
	}
	else if (height == 0.0f)
	{
		// XZ평면
		if (position.y > 0.0f)
		{
			v.position = { -hx, +hy, -hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, +hy, -hz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, +hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);

			v.position = { -hx, +hy, -hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, +hy, +hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
			v.position = { -hx, +hy, +hz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);
		}
		else
		{
			v.position = { +hx, +hy, -hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
			v.position = { -hx, +hy, -hz }; v.uv0 = { 0.0f, 1.0f }; vertices.push_back(v);
			v.position = { -hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);

			v.position = { +hx, +hy, -hz }; v.uv0 = { 1.0f, 1.0f }; vertices.push_back(v);
			v.position = { -hx, +hy, +hz }; v.uv0 = { 0.0f, 0.0f }; vertices.push_back(v);
			v.position = { +hx, +hy, +hz }; v.uv0 = { 1.0f, 0.0f }; vertices.push_back(v);
		}
	}

	CreateVertexBuffer(device, commandList, vertices.data(), sizeof(Vertex), vertices.size());
}

BillboardMesh::BillboardMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const XMFLOAT3& position, const XMFLOAT2& size)
{
	m_nIndices = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_POINTLIST;

	Vertex v;
	v.position = position;
	v.uv0 = size;
	CreateVertexBuffer(device, commandList, &v, sizeof(Vertex), 1);
}