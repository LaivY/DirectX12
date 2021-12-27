#pragma once

// C/C++
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
using namespace std;

// DIRECTX
#include <DirectXMath.h>
using namespace DirectX;

// FBX SDK
#include <fbxsdk.h>

struct Vertex
{
	XMFLOAT3 position;	// 위치
	XMFLOAT3 normal;	// 노말
	XMFLOAT2 uv;		// 텍스쳐 좌표
};

class FBXExporter
{
public:
	FBXExporter();
	~FBXExporter();

	void Process(const string& fbxFileName);
	void LoadNode(FbxNode* node);
	void LoadMesh(FbxMesh* mesh);
	void LoadAnimation(FbxNode* node);

	XMFLOAT3 GetNormal(FbxMesh* mesh, int controlPointIndex, int vertexCountIndex);
	XMFLOAT2 GetUV(FbxMesh* mesh, int controlPointIndex, int vertexCountIndex);

	void Export();

private:
	FbxManager*		m_manager;
	FbxScene*		m_scene;

	string			m_name;
	vector<Vertex>	m_vertices;
};