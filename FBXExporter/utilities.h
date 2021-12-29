#pragma once
#include <string>
#include <vector>
#include <DirectXMath.h>
#include <fbxsdk.h>
using namespace DirectX;
using namespace std;

// https://github.com/lang1991/FBXExporter

struct BlendingDatum
{
	BlendingDatum() : blendingIndex{}, blendingWeight{}
	{

	}

	int		blendingIndex;
	double	blendingWeight;
};

// 제어점
// 각 제어점은 위치와 4개의 블렌딩 정보를 갖는다.
struct CtrlPoint
{
	CtrlPoint() : position{}
	{
		blendingData.reserve(4);
	}

	XMFLOAT3				position;
	vector<BlendingDatum>	blendingData;
};

// 키프레임
struct Keyframe
{
	Keyframe() : frameNum{}
	{
		globalTransformMatrix.SetIdentity();
	}

	FbxLongLong frameNum;
	FbxAMatrix	globalTransformMatrix;
};

struct Joint
{
	Joint() : name{}, parentIndex{ -1 }, node{ nullptr }
	{
		globalBindposeInverseMatrix.SetIdentity();
	}

	string				name;							// 이름
	int					parentIndex;					// 부모 인덱스
	FbxAMatrix			globalBindposeInverseMatrix;	// 로컬 -> 월드좌표계 변환 행렬
	FbxNode*			node;							// 노드
	vector<Keyframe>	keyframes;						// 애니메이션 키프레임
};

struct Skeleton
{
	vector<Joint> joints;
};

// 정점
// 내가 클라이언트에서 사용할 정점 구조체
struct Vertex
{
	Vertex() : position{}, normal{}, uv{}
	{

	}

	XMFLOAT3				position;		// 위치
	XMFLOAT3				normal;			// 노말
	XMFLOAT2				uv;				// 텍스쳐 좌표
	vector<BlendingDatum>	blendingData;	// 가중치
};

namespace Utilities
{
	inline FbxAMatrix GetGeometryTransformation(FbxNode* node)
	{
		return FbxAMatrix(
			node->GetGeometricTranslation(FbxNode::eSourcePivot),
			node->GetGeometricRotation(FbxNode::eSourcePivot),
			node->GetGeometricScaling(FbxNode::eSourcePivot)
		);
	}
}