#pragma once
#include "stdafx.h"
#include "object.h"
#include "player.h"
#include "camera.h"
#include "terrain.h"
#include "skybox.h"

class Scene
{
public:
	Scene();
	~Scene();

	void OnInit(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& commandList, const ComPtr<ID3D12RootSignature>& rootSignature, FLOAT aspectRatio);
	void OnMouseEvent(HWND hWnd, UINT width, UINT height, FLOAT deltaTime) const;
	void OnKeyboardEvent(FLOAT deltaTime) const;
	void OnUpdate(FLOAT deltaTime);
	void Update(FLOAT deltaTime);
	void Render(const ComPtr<ID3D12GraphicsCommandList>& commandList) const;
	void ReleaseUploadBuffer();

	void AddGameObject(unique_ptr<GameObject> gameObject);
	void AddTerrain(unique_ptr<HeightMapTerrain> terrain);
	void SetSkybox(unique_ptr<Skybox>& skybox);
	void SetPlayer(const shared_ptr<Player>& player);
	void SetCamera(const shared_ptr<Camera>& camera);

	Skybox* GetSkybox() const { return m_skybox.get(); }
	shared_ptr<Player> GetPlayer() const { return m_player; }
	shared_ptr<Camera> GetCamera() const { return m_camera; }
	HeightMapTerrain* GetTerrain(FLOAT x, FLOAT z) const;

private:
	vector<unique_ptr<GameObject>>			m_gameObjects;
	vector<unique_ptr<HeightMapTerrain>>	m_terrains;
	unique_ptr<Skybox>						m_skybox;
	shared_ptr<Player>						m_player;
	shared_ptr<Camera>						m_camera;
};