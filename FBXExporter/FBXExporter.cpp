#include "FBXExporter.h"

FBXExporter::FBXExporter() : m_scene{ nullptr }
{
	// Fbx 매니저 생성
	m_manager = FbxManager::Create();

	// IOSettings 생성 및 설정
	FbxIOSettings* ioSettings{ FbxIOSettings::Create(m_manager, IOSROOT) };
	m_manager->SetIOSettings(ioSettings);
}

FBXExporter::~FBXExporter()
{
	if (m_manager) m_manager->Destroy();
}

void FBXExporter::Process(const string& fbxFileName)
{
	// 기존 씬 초기화
	if (m_scene) m_scene->Destroy();

	// 씬 생성
	m_scene = FbxScene::Create(m_manager, "SCENE");

	// 출력 파일 이름 설정
	m_name = fbxFileName;

	// 임포터 생성 및 초기화
	FbxImporter* fbxImporter{ FbxImporter::Create(m_manager, "IMPORTER") };
	if (!fbxImporter->Initialize(fbxFileName.c_str(), -1, m_manager->GetIOSettings()))
	{
		cout << "FbxImporter 초기화 실패" << endl;
		cout << fbxImporter->GetStatus().GetErrorString() << endl;
		exit(-1);
	}

	// 씬 로드 후 임포터 삭제
	fbxImporter->Import(m_scene);
	fbxImporter->Destroy();

	// 루트 노드 생성
	FbxNode* rootNode{ m_scene->GetRootNode() };

	// 가능한 모든 노드 삼각형화
	FbxGeometryConverter geometryConverter{ m_manager };
	geometryConverter.Triangulate(m_scene, true);

	// 노드를 순회하면서 데이터 로딩
	LoadNode(rootNode);

	// 데이터 출력
	Export();
}

void FBXExporter::LoadNode(FbxNode* node)
{
	if (!node) return;

	FbxMesh* mesh{ node->GetMesh() };
	if (mesh)
	{
		LoadMesh(node->GetMesh());
		LoadAnimation(node);
	}

	// 모든 노드 순회
	for (int i = 0; i < node->GetChildCount(); ++i)
		LoadNode(node->GetChild(i));
}

void FBXExporter::LoadMesh(FbxMesh* mesh)
{
	/*
	* 제어점과 정점의 차이점
	* 제어점은 해당 메쉬를 이루는 점의 개수
	* 정점은 해당 메쉬를 이루는 삼각형의 점의 개수
	* ex) 사각형 메쉬가 있다고 하면 제어점은 4개, 정점은 6개이다.
	*/

	int nControlPoint{ mesh->GetControlPointsCount() };	// 제어점 개수
	int nTriangle{ mesh->GetPolygonCount() };			// 삼각형 개수
	int nVertices{ 0 };									// 정점 개수

	// 갖고있던 정점 데이터 초기화
	m_vertices.clear();

	// 제어점
	vector<XMFLOAT3> controlPoints(nControlPoint);
	for (int i = 0; i < nControlPoint; ++i)
	{
		FbxVector4 pos{ mesh->GetControlPointAt(i) };
		controlPoints.emplace_back(pos[0], pos[1], pos[2]);
	}

	// 인덱스
	for (int i = 0; i < nTriangle; ++i)
	{
		for (int j = 0; j < mesh->GetPolygonSize(i); ++j) // 삼각형은 3개의 정점으로 이루어짐
		{
			// i번째 삼각형의 j번째 정점 인덱스
			int controlPointIndex{ mesh->GetPolygonVertex(i, j) };

			Vertex vertex{};
			vertex.position = controlPoints[controlPointIndex];
			vertex.normal = GetNormal(mesh, controlPointIndex, nVertices);
			vertex.uv = GetUV(mesh, controlPointIndex, nVertices);
			m_vertices.push_back(vertex);
			++nVertices;
		}
	}
}

XMFLOAT3 FBXExporter::GetNormal(FbxMesh* mesh, int controlPointIndex, int vertexCountIndex)
{
	FbxVector4 result{};
	if (mesh->GetElementNormalCount() < 1)
		return XMFLOAT3{};

	FbxGeometryElementNormal* normal{ mesh->GetElementNormal(0) };
	if (normal->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			result = normal->GetDirectArray().GetAt(controlPointIndex);
		}
		else if (normal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			int index{ normal->GetIndexArray().GetAt(controlPointIndex) };
			result = normal->GetDirectArray().GetAt(index);
		}
	}
	else if (normal->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (normal->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			result = normal->GetDirectArray().GetAt(vertexCountIndex);
		}
		else if (normal->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			int index{ normal->GetIndexArray().GetAt(vertexCountIndex) };
			result = normal->GetDirectArray().GetAt(index);
		}
	}
	return XMFLOAT3(result[0], result[1], result[2]);
}

XMFLOAT2 FBXExporter::GetUV(FbxMesh* mesh, int controlPointIndex, int vertexCountIndex)
{
	FbxVector2 result{};
	if (mesh->GetElementUVCount() < 1)
		return XMFLOAT2{};

	FbxGeometryElementUV* uv{ mesh->GetElementUV(0) };
	if (uv->GetMappingMode() == FbxGeometryElement::eByControlPoint)
	{
		if (uv->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			result = uv->GetDirectArray().GetAt(controlPointIndex);
		}
		else if (uv->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			int index{ uv->GetIndexArray().GetAt(controlPointIndex) };
			result = uv->GetDirectArray().GetAt(index);
		}
	}
	else if (uv->GetMappingMode() == FbxGeometryElement::eByPolygonVertex)
	{
		if (uv->GetReferenceMode() == FbxGeometryElement::eDirect)
		{
			result = uv->GetDirectArray().GetAt(vertexCountIndex);
		}
		else if (uv->GetReferenceMode() == FbxGeometryElement::eIndexToDirect)
		{
			int index{ uv->GetIndexArray().GetAt(vertexCountIndex) };
			result = uv->GetDirectArray().GetAt(index);
		}
	}
	return XMFLOAT2(result[0], result[1]);
}

void FBXExporter::LoadAnimation(FbxNode* node)
{
	// 모든 노드 순회
	for (int i = 0; i < node->GetChildCount(); ++i)
		LoadAnimation(node->GetChild(i));
}

void FBXExporter::Export()
{
	ofstream f{ "result.bin", ios::binary };

	size_t nVertices{ m_vertices.size() };
	f.write(reinterpret_cast<char*>(&nVertices), sizeof(size_t)); // 정점 개수

	for (Vertex& v : m_vertices)
	{
		f.write(reinterpret_cast<char*>(&v.position.x), sizeof(float));
		f.write(reinterpret_cast<char*>(&v.position.y), sizeof(float));
		f.write(reinterpret_cast<char*>(&v.position.z), sizeof(float));

		f.write(reinterpret_cast<char*>(&v.normal.x), sizeof(float));
		f.write(reinterpret_cast<char*>(&v.normal.y), sizeof(float));
		f.write(reinterpret_cast<char*>(&v.normal.z), sizeof(float));

		f.write(reinterpret_cast<char*>(&v.uv.x), sizeof(float));
		f.write(reinterpret_cast<char*>(&v.uv.y), sizeof(float));
	}
	f.close();

	/*
	ifstream iFile{ "result.bin", ios::binary };

	size_t n{};
	iFile.read(reinterpret_cast<char*>(&n), sizeof(size_t));

	for (int i = 0; i < n; ++i)
	{
		Vertex v{};
		iFile.read(reinterpret_cast<char*>(&v.position), sizeof(XMFLOAT3));
		iFile.read(reinterpret_cast<char*>(&v.normal), sizeof(XMFLOAT3));
		iFile.read(reinterpret_cast<char*>(&v.uv), sizeof(XMFLOAT2));
		cout << i << endl;
		cout << v.position.x << ", " << v.position.y << ", " << v.position.z << endl;
		cout << v.normal.x << ", " << v.normal.y << ", " << v.normal.z << endl;
		cout << v.uv.x << ", " << v.uv.y << endl << endl;
	}
	*/
}