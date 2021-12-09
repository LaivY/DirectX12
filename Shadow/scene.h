#pragma once
#include "stdafx.h"
#include "camera.h"
#include "object.h"
#include "player.h"
#include "shadow.h"
#include "skybox.h"
#include "terrain.h"

struct Light // 16바이트로 정렬
{
	XMFLOAT3	strength;					// 색상					:: 12
	FLOAT		fallOffStart;				// 감쇠 시작 거리		:: 4
	XMFLOAT3	direction;					// 방향					:: 12
	FLOAT		fallOffEnd;					// 감쇠 끝 거리			:: 4
	XMFLOAT3	position;					// 위치					:: 12
	bool		isActivate;					// 활성화 여부			:: 4
	int			type;						// 0 : 방향, 1 : 점		:: 4
	XMFLOAT3	padding;					// 채우기용				:: 12
};

struct Material // 16바이트로 정렬
{
	XMFLOAT4	diffuseAlbedo;				// 분산 반사율(전체적인 색감)
	XMFLOAT3	fresnelR0;					// 반사광
	FLOAT		roughness;					// 표면의 거칠기
};

struct cbScene
{
	Light		ligths[MAX_LIGHT];			// 조명들
	Material	materials[MAX_MATERIAL];	// 재질들
	XMFLOAT4X4	lightViewMatrix;			// 그림자를 만드는 조명 뷰 변환 행렬
	XMFLOAT4X4	lightProjMatrix;			// 그림자를 만드는 조명 투영 변환 행렬
	XMFLOAT4X4	NDCToTextureMatrix;			// NDC -> 텍스쳐 좌표계 변환 행렬
};

class ResourceManager
{
public:
	ResourceManager() = default;
	~ResourceManager() = default;

	void ReleaseUploadBuffer() const;

	void AddMesh(const string& key, const shared_ptr<Mesh>& mesh) { m_meshes[key] = mesh; }
	void AddShader(const string & key, const shared_ptr<Shader>&shader) { m_shaders[key] = shader; }
	void AddTexture(const string & key, const shared_ptr<Texture>&texture) { m_textures[key] = texture; }

	shared_ptr<Mesh> GetMesh(const string & key) const;
	shared_ptr<Shader> GetShader(const string & key) const;
	shared_ptr<Texture> GetTexture(const string & key) const;

private:
	map<string, shared_ptr<Mesh>>		m_meshes;
	map<string, shared_ptr<Shader>>		m_shaders;
	map<string, shared_ptr<Texture>>	m_textures;
};

class Scene
{
public:
	Scene();
	~Scene();

	void OnInit(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const ComPtr<ID3D12RootSignature>& rootSignature, FLOAT aspectRatio);
	void OnMouseEvent(HWND hWnd, UINT width, UINT height, FLOAT deltaTime);
	void OnMouseEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnKeyboardEvent(FLOAT deltaTime);
	void OnKeyboardEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	void OnUpdate(FLOAT deltaTime);

	void CreateShaderVariable(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList);
	void UpdateShaderVariable(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void CreateLightAndMeterial();
	void CreateBullet();

	void Update(FLOAT deltaTime);
	void RemoveDeletedObjects();
	void UpdateObjectsTerrain();

	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList, D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) const;
	void RenderMirror(const ComPtr<ID3D12GraphicsCommandList>& commandList, D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle) const;
	void RenderShadowMap(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;

	void ReleaseUploadBuffer();

	void SetSkybox(unique_ptr<Skybox>& skybox);
	void SetPlayer(const shared_ptr<Player>& player);
	void SetCamera(const shared_ptr<Camera>& camera);

	Skybox* GetSkybox() const { return m_skybox.get(); }
	shared_ptr<Player> GetPlayer() const { return m_player; }
	shared_ptr<Camera> GetCamera() const { return m_camera; }
	HeightMapTerrain* GetTerrain(FLOAT x, FLOAT z) const;

private:
	D3D12_VIEWPORT							m_viewport;
	D3D12_RECT								m_scissorRect;

	unique_ptr<ResourceManager>				m_resourceManager;	// 모든 메쉬, 셰이더, 텍스쳐들

	vector<unique_ptr<GameObject>>			m_gameObjects;		// 게임오브젝트
	vector<unique_ptr<GameObject>>			m_particles;		// 반투명 객체
	vector<unique_ptr<HeightMapTerrain>>	m_terrains;			// 지형
	unique_ptr<GameObject>					m_mirror;			// 거울
	unique_ptr<Skybox>						m_skybox;			// 스카이박스
	shared_ptr<Player>						m_player;			// 플레이어
	shared_ptr<Camera>						m_camera;			// 카메라

	unique_ptr<ShadowMap>					m_shadowMap;		// 그림자맵

	ComPtr<ID3D12Resource>					m_cbScene;			// 씬 상수 버퍼
	cbScene*								m_pcbScene;			// 씬 상수 버퍼 포인터
	unique_ptr<cbScene>						m_cbSceneData;		// 씬 상수 버퍼 데이터
};