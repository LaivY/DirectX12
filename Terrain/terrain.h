#pragma once
#include "stdafx.h"
#include "object.h"
#include "mesh.h"

class HeightMapImage
{
public:
	HeightMapImage(const wstring& fileName, INT width, INT length, XMFLOAT3 scale);
	~HeightMapImage() = default;

	BYTE* GetPixels() const { return m_pixels.get(); }
	XMFLOAT3 GetNormal(INT x, INT z) const;
	FLOAT GetHeight(FLOAT x, FLOAT z) const;
	INT GetWidth() const { return m_width; }
	INT GetLength() const { return m_length; }
	XMFLOAT3 GetScale() const { return m_scale; }
	
private:
	unique_ptr<BYTE[]>	m_pixels;	// �ȼ����� 2���� �迭(�� ���Ҵ� 0~255�� ��)
	INT					m_width;	// �̹����� ���� ����
	INT					m_length;	// �̹����� ���� ����
	XMFLOAT3			m_scale;	// Ȯ�� ����
};

class HeightMapGridMesh : public Mesh
{
public:
	HeightMapGridMesh(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, 
		HeightMapImage* heightMapImage, INT xStart, INT zStart, INT width, INT length, XMFLOAT3 scale);
	~HeightMapGridMesh() = default;

	FLOAT GetHeight(HeightMapImage* heightMapImage, INT x, INT z) const;

private:
	INT			m_width;	// ����
	INT			m_length;	// ����
	XMFLOAT3	m_scale;	// Ȯ�� ����
};

class HeightMapTerrain
{
public:
	HeightMapTerrain(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList,
		const wstring& fileName, const shared_ptr<Shader>& shader, const shared_ptr<Texture>& texture, INT width, INT length, INT blockWidth, INT blockLength, XMFLOAT3 scale);
	~HeightMapTerrain() = default;

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	FLOAT GetHeight(FLOAT x, FLOAT z) const { return m_heightMapImage->GetHeight(x / m_scale.x, z / m_scale.z) * m_scale.y; };
	INT GetImageWidth() const { return m_heightMapImage->GetWidth(); }
	INT GetImageLength() const { return m_heightMapImage->GetLength(); }

private:
	shared_ptr<HeightMapImage>		m_heightMapImage;	// ���̸� �̹���
	vector<unique_ptr<GameObject>>	m_blocks;			// ���ϵ�
	INT								m_width;			// ����
	INT								m_length;			// ����
	XMFLOAT3						m_scale;			// Ȯ�� ����
};