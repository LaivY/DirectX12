#include "terrain.h"
#include <fstream>

HeightMapImage::HeightMapImage(const wstring& fileName, INT width, INT length, XMFLOAT3 scale)
	: m_width{ width }, m_length{ length }, m_scale{ scale }, m_pixels{ new BYTE[width * length] }
{
	// 파일 읽기
	unique_ptr<BYTE[]> buffer{ new BYTE[m_width * m_length] };
	HANDLE hFile{ CreateFile(fileName.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_READONLY, NULL) };
	DWORD bytesRead;
	ReadFile(hFile, buffer.get(), m_width * m_length, &bytesRead, NULL);
	CloseHandle(hFile);

	// 높이맵 이미지는 좌상단이 (0, 0)이고 우리가 원하는 좌표계는 좌하단이 (0, 0)이므로 상하대칭 시켜서 저장한다.
	for (int y = 0; y < m_length; ++y)
		for (int x = 0; x < m_width; ++x)
			m_pixels[x + (y * m_width)] = buffer[x + ((m_length - y - 1) * m_width)];
}

XMFLOAT3 HeightMapImage::GetNormal(INT x, INT z) const
{
	// x, z좌표가 이미지의 범위에 벗어난 경우 +y축을 반환
	if (x < 0 || x >= m_width || z < 0 || z >= m_length)
		return XMFLOAT3{ 0.0f, 1.0f, 0.0f };

	// P1(x, z), P2(x+1, z), P3(x, z+1) 점 3개를 이용해서 법선 벡터를 계산한다.
	int index{ x + z * m_width };
	int xAdd{ x < m_width - 1 ? 1 : -1 };				// x가 가장 오른쪽 픽셀일 경우 (x-1, z)를 이용
	int yAdd{ z < m_length - 1 ? m_width : -m_width };	// z가 가장 밑일 경우 (x, z-1)을 이용

	BYTE* pixels{ m_pixels.get() };
	float y1{ pixels[index] * m_scale.y };			// P1의 y값
	float y2{ pixels[index + xAdd] * m_scale.y };	// P2의 y값
	float y3{ pixels[index + yAdd] * m_scale.y };	// P3의 y값

	XMFLOAT3 P1P2{ m_scale.x, y2 - y1, 0.0f }; // P1 -> P2 벡터
	XMFLOAT3 P1P3{ 0.0f, y3 - y1, m_scale.z }; // P1 -> P3 벡터

	// 두 벡터를 외적한 것이 법선 벡터이다.
	return Vector3::Normalize(Vector3::Cross(P1P3, P1P2));
}

FLOAT HeightMapImage::GetHeight(FLOAT x, FLOAT z) const
{
	// x, z좌표가 이미지의 범위에 벗어난 경우 0을 반환
	if (x < 0 || x >= m_width || z < 0 || z >= m_length)
		return 0.0f;

	int ix{ (int)x };	// x의 정수 부분
	int iz{ (int)z };	// z의 정수 부분
	float fx{ x - ix };	// x의 소수 부분
	float fz{ z - iz };	// z의 소수 부분

	BYTE* pixels{ m_pixels.get() };
	float LT{ (float)pixels[ix + ((iz + 1) * m_width)] };		// 좌상단 높이
	float RT{ (float)pixels[(ix + 1) + ((iz + 1) * m_width)] };	// 우상단 높이
	float LB{ (float)pixels[ix + (iz * m_width)] };				// 좌하단 높이
	float RB{ (float)pixels[(ix + 1) + (iz * m_width)] };		// 우하단 높이

	// 사각형의 네 점을 보간하여 최종 높이 반환
	float topHeight{ LT * (1 - fx) + RT * fx };		// 보간한 상단 높이
	float botHeight{ LB * (1 - fx) + RB * fx };		// 보간한 하단 높이
	return botHeight * (1 - fz) + topHeight * fz;	// 보간한 최종 높이
}

// --------------------------------------

HeightMapGridMesh::HeightMapGridMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	HeightMapImage* heightMapImage, INT xStart, INT zStart, INT width, INT length, XMFLOAT3 scale)
	: m_width{ width }, m_length{ length }, m_scale{ scale }
{
	// 정점 데이터 설정, 정점 버퍼 생성
	vector<TextureVertex> vertices;
	for (int z = zStart; z < zStart + m_length; ++z)
		for (int x = xStart; x < xStart + m_width; ++x)
			vertices.emplace_back(
				XMFLOAT3{ x * m_scale.x, heightMapImage->GetHeight(x, z) * m_scale.y, z * m_scale.z },
				XMFLOAT2{ (float)x / (float)heightMapImage->GetWidth(), 1.0f - ((float)z / (float)heightMapImage->GetLength()) });
	CreateVertexBuffer(device, commandList, vertices.data(), sizeof(TextureVertex), vertices.size());

	// 인덱스 데이터 설정, 인덱스 버퍼 생성
	vector<UINT> indices;
	for (int z = 0; z < m_length - 1; ++z) // 마지막 번 째 줄은 할 필요 없음
	{
		// 홀수 번째 줄 (z = 0, 2, 4, 6, ...) 인덱스 나열 순서는 왼쪽 -> 오른쪽
		if (z % 2 == 0)
			for (int x = 0; x < m_width; ++x)
			{
				// 첫번째 줄이 아니고 줄이 바뀔 때 (x, z)를 추가
				if (x == 0 && z > 0) indices.push_back(x + (z * m_width));
				indices.push_back(x + (z * m_width));			// (x, z)
				indices.push_back(x + (z * m_width) + m_width);	// (x, z+1)
			}

		// 짝수 번째 줄 (z = 1, 3, 5, 7, ...) 인덱스 나열 순서는 왼쪽 <- 오른쪽
		else
			for (int x = m_width - 1; x >= 0; --x)
			{
				// 줄이 바뀔 때 (x, z)를 추가
				if (x == m_width - 1) indices.push_back(x + (z * m_width));
				indices.push_back(x + (z * m_width));			// (x, z)
				indices.push_back(x + (z * m_width) + m_width);	// (x, z+1)
			}
	}
	CreateIndexBuffer(device, commandList, indices.data(), indices.size());
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
}

FLOAT HeightMapGridMesh::GetHeight(HeightMapImage* heightMapImage, INT x, INT z) const
{
	BYTE* pixels{ heightMapImage->GetPixels() };
	return pixels[x + z * heightMapImage->GetWidth()] * heightMapImage->GetScale().y;
}

// --------------------------------------

HeightMapTerrain::HeightMapTerrain(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
	const wstring& fileName, const shared_ptr<Shader>& shader, const shared_ptr<Texture>& texture, INT width, INT length, INT blockWidth, INT blockLength, XMFLOAT3 scale)
	: m_width{ width }, m_length{ length }, m_scale{ scale }
{
	// 높이맵이미지 로딩
	m_heightMapImage = make_unique<HeightMapImage>(fileName, m_width, m_length, m_scale);

	// 가로, 세로 블록의 개수
	int widthBlockCount{ m_width / blockWidth };
	int lengthBlockCount{ m_length / blockLength };

	// 블록 생성
	for (int z = 0; z < lengthBlockCount; ++z)
		for (int x = 0; x < widthBlockCount; ++x)
		{
			int xStart{ x * (blockWidth - 1) };
			int zStart{ z * (blockLength - 1) };
			unique_ptr<GameObject> block{ make_unique<GameObject>() };
			shared_ptr<HeightMapGridMesh> mesh{
				make_shared<HeightMapGridMesh>(device, commandList, m_heightMapImage.get(), xStart, zStart, blockWidth, blockLength, m_scale)
			};
			block->SetMesh(mesh);
			block->SetShader(shader);
			block->SetTexture(texture);
			m_blocks.push_back(move(block));
		}
}

void HeightMapTerrain::Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const
{
	for (const auto& obj : m_blocks)
		obj->Render(commandList);
}