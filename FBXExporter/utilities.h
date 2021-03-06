#pragma once
#include <DirectXMath.h>
#include <fbxsdk.h>
using namespace DirectX;
using namespace std;

namespace Utilities
{
	inline string GetFilePath(const string& path)
	{
		size_t p{ path.find_last_of('/') };
		return path.substr(0, p + 1);
	}
	inline string GetFileName(const string& path)
	{
		size_t p1{ path.find_last_of('/') };
		size_t p2{ path.find_last_of('.') };
		return path.substr(p1 + 1, p2 - p1 - 1);
	}
	inline FbxAMatrix GetGeometryTransformation(FbxNode* node)
	{
		return FbxAMatrix(
			node->GetGeometricTranslation(FbxNode::eSourcePivot),
			node->GetGeometricRotation(FbxNode::eSourcePivot),
			node->GetGeometricScaling(FbxNode::eSourcePivot)
		);
	}
	inline FbxAMatrix toFbxAMatrix(const XMFLOAT4X4 m)
	{
		FbxAMatrix result;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				result[i][j] = m.m[i][j];
		return result;
	}
	inline FbxAMatrix toFbxAMatrix(const XMMATRIX& m)
	{
		FbxAMatrix result;
		XMFLOAT4X4 matrix;
		XMStoreFloat4x4(&matrix, m);
		return toFbxAMatrix(matrix);
	}
	inline XMFLOAT4X4 toXMFLOAT4X4(const FbxAMatrix& m)
	{
		XMFLOAT4X4 result;
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				result.m[i][j] = m[i][j];
		return result;
	}
	inline XMFLOAT4 FbxVector4ToXMFLOAT4(const FbxVector4& v)
	{
		return XMFLOAT4{ 
			static_cast<float>(v.mData[0]),
			static_cast<float>(v.mData[1]),
			static_cast<float>(v.mData[2]),
			static_cast<float>(v.mData[3])
		};
	}
	inline XMFLOAT4X4 Identity()
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMMatrixIdentity());
		return result;
	}
	inline XMFLOAT4X4 Transpose(const XMFLOAT4X4& m)
	{
		XMFLOAT4X4 result;
		XMStoreFloat4x4(&result, XMMatrixTranspose(XMLoadFloat4x4(&m)));
		return result;
	}
	inline void WriteToStream(ostream& s, const FbxAMatrix& m)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				s << static_cast<float>(m.Get(i, j)) << " ";
	}
	inline void WriteToStream(ostream& s, const XMFLOAT4X4& m)
	{
		for (int i = 0; i < 4; ++i)
			for (int j = 0; j < 4; ++j)
				s << m.m[i][j] << " ";
	}
	inline void Print(const FbxAMatrix& m)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				cout << m.Get(i, j) << " ";
			cout << endl;
		}
	}
	inline void Print(const XMFLOAT4X4& m)
	{
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
				cout << m.m[i][j] << " ";
			cout << endl;
		}
	}
}